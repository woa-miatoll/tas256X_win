#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AudFilterCreateDevice)
#endif

PDEVICE_CONTEXT g_pDevice;

VOID ClearTimerStatus(
    _In_ WDFTIMER Timer
)
{
    UNREFERENCED_PARAMETER(Timer);

    // Reset Timer status after 8s
    // Must call this function after g_pDevice initializing
    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_DEVICE,
        "Time Reached. Call Count: 0x%llx", g_pDevice->CallCount);
    g_pDevice->TIMER_STATUS = FALSE;
}

NTSTATUS
OnReleaseHardware(
    _In_  WDFDEVICE     FxDevice,
    _In_  WDFCMRESLIST  FxResourcesTranslated
) {
    UNREFERENCED_PARAMETER(FxResourcesTranslated);
    UNREFERENCED_PARAMETER(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    if (g_pDevice->CSAudioAPICallbackObj != NULL)
        ExUnregisterCallback(g_pDevice->CSAudioAPICallbackObj);

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
AudFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
    pnpCallbacks.EvtDeviceReleaseHardware = OnReleaseHardware;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpCallbacks);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    g_pDevice = DeviceGetContext(device);
    g_pDevice->CallCount = 0;

    // CS Audio Callback
    UNICODE_STRING CSAudioCallbackAPI;
    RtlInitUnicodeString(&CSAudioCallbackAPI, L"\\CallBack\\CsAudioCallbackAPI");

    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(&attributes,
        &CSAudioCallbackAPI,
        OBJ_KERNEL_HANDLE | OBJ_OPENIF | OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
        NULL,
        NULL
    );

    status = ExCreateCallback(&g_pDevice->CSAudioAPICallback, &attributes, TRUE, TRUE);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    g_pDevice->CSAudioAPICallbackObj = ExRegisterCallback(g_pDevice->CSAudioAPICallback,
        CsAudioCallbackFunction,
        g_pDevice
    );

    if (!g_pDevice->CSAudioAPICallbackObj) {
        return STATUS_NO_CALLBACK_ACTIVE;
    }

    //
    // Create a parallel dispatch queue to handle requests from HID Class
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = OnDeviceControl;
    queueConfig.EvtIoStop = EvtIoStop;
    queueConfig.EvtIoResume = EvtIoResume;;

    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        WDF_NO_HANDLE); // pointer to default queue

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "Error creating WDF default queue - 0x%08lX",
            status);

        goto exit;
    }

    // Timer Init
    WDF_TIMER_CONFIG timerConfig = {0};
    WDF_OBJECT_ATTRIBUTES timerAttributes = { 0 };
    WDF_TIMER_CONFIG_INIT(&timerConfig, ClearTimerStatus);
    timerConfig.UseHighResolutionTimer = FALSE;
    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = device;
    status = WdfTimerCreate(&timerConfig, &timerAttributes, &g_pDevice->Timer);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DEVICE,
        "Timer Created Status:0x%08lX",
        status);

exit:
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}

int CsAudioArg2 = 1;

VOID
CsAudioCallbackFunction(
    PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
) {
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(Argument2);
    UNREFERENCED_PARAMETER(arg);
}

VOID
CSAudioNotifyEndpoint(
    CSAudioEndpointRequest RequestAction
) {
    CsAudioArg arg;
    RtlZeroMemory(&arg, sizeof(CsAudioArg));
    arg.argSz = sizeof(CsAudioArg);
    arg.endpointType = CSAudioEndpointTypeSpeaker;
    arg.endpointRequest = RequestAction;
    ExNotifyCallback(g_pDevice->CSAudioAPICallback, &arg, &CsAudioArg2);
}

VOID
OnDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
)
{
    NTSTATUS status;
    WDFDEVICE device;
    BOOLEAN CheckAmpStatus = FALSE;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    WDFIOTARGET Target;
    PIRP Irp = NULL;
    PIO_STACK_LOCATION IrpSp;

    UNREFERENCED_PARAMETER(OutputBufferLength);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);

    Target = WdfDeviceGetIoTarget(device);

    Irp = WdfRequestWdmGetIrp(Request);
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    if (IoControlCode == 0x2f0003
        && InputBufferLength == 0x18
        && (*(UINT64*)IrpSp->Parameters.DeviceIoControl.Type3InputBuffer == 0x11cfac9b1d58c920))
    {
        CheckAmpStatus = TRUE;
        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DEVICE,
            "Audio state change.");
    }

    //
    // Forward the request down. WdfDeviceGetIoTarget returns
    // the default target, which represents the device attached to us below in
    // the stack.
    //
    if (CheckAmpStatus && !g_pDevice->TIMER_STATUS) {
        // Signal Callback func in TAS driver.
        CSAudioNotifyEndpoint(CSAudioEndpointStart);
        g_pDevice->TIMER_STATUS = TRUE;
        status = WdfTimerStart(g_pDevice->Timer, WDF_REL_TIMEOUT_IN_SEC(8));
        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DEVICE,
            "Timer Started Status:0x%08lX",
            status);
    }

    //
    // We are not interested in processing the IRP so
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
        &options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, Target, &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus(Request);

        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}

VOID
EvtIoStop(
    WDFQUEUE Queue,
    WDFREQUEST Request,
    ULONG ActionFlags
)
{
    NTSTATUS status;
    WDFDEVICE device;
    //BOOLEAN forwardWithCompletionRoutine = FALSE;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    //WDFMEMORY memory;
    WDFIOTARGET Target;

    UNREFERENCED_PARAMETER(ActionFlags);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);
    Target = WdfDeviceGetIoTarget(device);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_DEVICE,
        "EvtWdfIoQueueIoStop Enter");

    // Mute Device Here.
    CSAudioNotifyEndpoint(CSAudioEndpointStop);

    //
    // We are not interested in post processing the IRP so
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
        &options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, Target, &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus(Request);

        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}

VOID
EvtIoResume(
    WDFQUEUE Queue,
    WDFREQUEST Request
)
{
    NTSTATUS status;
    WDFDEVICE device;
    //BOOLEAN forwardWithCompletionRoutine = FALSE;
    BOOLEAN requestSent = TRUE;
    WDF_REQUEST_SEND_OPTIONS options;
    //WDFMEMORY memory;
    WDFIOTARGET Target;

    // UNREFERENCED_PARAMETER(ActionFlags);

    PAGED_CODE();

    device = WdfIoQueueGetDevice(Queue);
    Target = WdfDeviceGetIoTarget(device);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_DEVICE,
        "EvtWdfIoQueueIoResume Enter");

    // Start Device Here.
    CSAudioNotifyEndpoint(CSAudioEndpointStart);

    //
    // We are not interested in post processing the IRP so
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
        &options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    requestSent = WdfRequestSend(Request, Target, &options);

    if (requestSent == FALSE) {
        status = WdfRequestGetStatus(Request);

        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DEVICE,
            "WdfRequestSend failed: 0x%x\n",
            status);

        WdfRequestComplete(Request, status);
    }

    return;
}

VOID
OnRequestCompletionRoutine(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
)
{
    NTSTATUS    status = Params->IoStatus.Status;
    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);

    WdfRequestComplete(Request, status);
    return;
}

VOID
FilterForwardRequest(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET Target
)
{
    WDF_REQUEST_SEND_OPTIONS Options;
    BOOLEAN                  RequestSent;
    NTSTATUS                 Status;

    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&Options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    RequestSent = WdfRequestSend(Request, Target, &Options);

    if (RequestSent == FALSE) {
        Status = WdfRequestGetStatus(Request);
        KdPrint(("WdfRequestSend failed: 0x%x\n", Status));
        WdfRequestComplete(Request, Status);
    }

    return;
}
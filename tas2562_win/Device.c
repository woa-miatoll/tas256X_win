#include "driver.h"
#include "spb.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, Tas2562CreateDevice)
#endif

UINT8 g_MuteRegister = 0x02;
UINT8 g_DeviceMute = 0; // 2: Software Shutdown, 1: Mute, 0: Active


BOOLEAN
CheckMuteStatus(
    PDEVICE_CONTEXT pDevice
) {
    PAGED_CODE();
    SpbDeviceWriteRead(&pDevice->SpbContextA, &g_MuteRegister, &g_DeviceMute, sizeof(g_MuteRegister), sizeof(g_DeviceMute));
    return g_DeviceMute != 0;
}

#define WA(...) {                                                                                           \
    SpbDeviceWrite(&pDevice->SpbContextA, (UINT8[]){ __VA_ARGS__ }, sizeof((UINT8[]){ __VA_ARGS__ }));  \
}

#define WB(...) {                                                                                           \
    SpbDeviceWrite(&pDevice->SpbContextB, (UINT8[]){ __VA_ARGS__ }, sizeof((UINT8[]){ __VA_ARGS__ }));  \
}

NTSTATUS
OnPrepareHardware(
    _In_  WDFDEVICE     FxDevice,
    _In_  WDFCMRESLIST  FxResourcesRaw,
    _In_  WDFCMRESLIST  FxResourcesTranslated
) {
    UNREFERENCED_PARAMETER(FxResourcesRaw);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN fSpbResourceFoundA = FALSE;
    BOOLEAN fSpbResourceFoundB = FALSE;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    ULONG resourceCount = WdfCmResourceListGetCount(FxResourcesTranslated);

    for (ULONG i = 0; i < resourceCount; i++) {
        PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor;
        UCHAR Class;
        UCHAR Type;

        pDescriptor = WdfCmResourceListGetDescriptor(FxResourcesTranslated, i);

        switch (pDescriptor->Type) {
        case CmResourceTypeConnection:
            Class = pDescriptor->u.Connection.Class;
            Type = pDescriptor->u.Connection.Type;

            if ((Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL) &&
                ((Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)))
            {
                if (fSpbResourceFoundB == FALSE && fSpbResourceFoundA == TRUE)
                {
                    pDevice->SpbContextB.I2cResHubId.LowPart =
                        pDescriptor->u.Connection.IdLowPart;
                    pDevice->SpbContextB.I2cResHubId.HighPart =
                        pDescriptor->u.Connection.IdHighPart;
                    fSpbResourceFoundB = TRUE;
                }
            }
            if ((Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL) &&
                ((Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)))
            {
                if (fSpbResourceFoundA == FALSE)
                {
                    pDevice->SpbContextA.I2cResHubId.LowPart =
                        pDescriptor->u.Connection.IdLowPart;
                    pDevice->SpbContextA.I2cResHubId.HighPart =
                        pDescriptor->u.Connection.IdHighPart;
                    fSpbResourceFoundA = TRUE;
                }
            }
            break;
        default:
            break;
        }
    }

    if (fSpbResourceFoundA == FALSE && fSpbResourceFoundB == FALSE)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    status = SpbDeviceOpen(FxDevice, &pDevice->SpbContextA);
    if(fSpbResourceFoundB)
        status = SpbDeviceOpen(FxDevice, &pDevice->SpbContextB);

    pDevice->TwoSpeakers = fSpbResourceFoundA && fSpbResourceFoundB;

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnReleaseHardware(
    _In_  WDFDEVICE     FxDevice,
    _In_  WDFCMRESLIST  FxResourcesTranslated
) {
    UNREFERENCED_PARAMETER(FxResourcesTranslated);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    if (pDevice->CSAudioAPICallbackObj) {
        ExUnregisterCallback(pDevice->CSAudioAPICallbackObj);
        pDevice->CSAudioAPICallbackObj = NULL;
    }

    if (pDevice->CSAudioAPICallback) {
        ObfDereferenceObject(pDevice->CSAudioAPICallback);
        pDevice->CSAudioAPICallback = NULL;
    }

    SpbDeviceClose(&pDevice->SpbContextA);
    if(pDevice->TwoSpeakers)
        SpbDeviceClose(&pDevice->SpbContextB);

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnD0Entry(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
) {
    UNREFERENCED_PARAMETER(FxPreviousState);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    WdfWaitLockAcquire(pDevice->StartLock, NULL);
    WA(0x00, 0x00); // Switch page to 0
    WA(0x7f, 0x00); // Set book 0
    WA(0x01, 0x00); // Reset
    WA(0x02, 0x00); // Set active mode
    WA(0x03, 0x20); // Set AMP Level to 16dBV
    WA(0x04, 0xcf);
    WA(0x06, 0x09); // Set sampler rate to 48khz
    WA(0x07, 0x03); // Set rx offset to 1
    // WA(0x08, 0x2a) Should be used instead of 0x1a but doesn't work right now for some reason
    WA(0x08, 0x1a); // Set rx word length to 24bits and rx time slot length to 32bits, ??set rx time slot select config to mono right channel??
    WA(0x0a, 0x11);
    WA(0x0b, 0x46);
    WA(0x0c, 0x44);
    WA(0x3e, 0x00); // Disable ICN
    WA(0x30, 0x39); // Disable CLK_HALT_EN
    WA(0x00, 0x02); // Switch page to 2
    WA(0x0c, 0x40); // Default volume
    WA(0x0d, 0x40); // Default volume
    WA(0x0e, 0x40); // Default volume
    WA(0x0f, 0x00); // Default volume
    WA(0x00, 0x00); // Switch page to 0
    WA(0x0b, 0x00); // Disable VSNS_TX
    WA(0x0c, 0x00); // Disable ISNS_TX

    if (pDevice->TwoSpeakers) {
        WB(0x00, 0x00); // Switch page to 0
        WB(0x7f, 0x00); // Set book 0
        WB(0x01, 0x00); // Reset
        WB(0x02, 0x00); // Set mode to active mode
        WB(0x03, 0x20); // Set AMP Level to 16dBV
        WB(0x06, 0x09); // Set sampler rate to 48khz
        WB(0x07, 0x03); // Set rx offset to 1
        WB(0x08, 0x1a); // Set rx word length to 24bits and rx time slot length to 32bits, set rx time slot select config to mono left channel
        WB(0x0a, 0xf1);
        WB(0x0b, 0x42);
        WB(0x0c, 0x40);
        WB(0x3e, 0x00); // Disable ICN
        WB(0x30, 0x39); // Disable CLK_HALT_EN
        WB(0x00, 0x02); // Switch page to 2
        WB(0x0c, 0x40); // Default volume
        WB(0x0d, 0x40); // Default volume
        WB(0x0e, 0x40); // Default volume
        WB(0x0f, 0x00); // Default volume
        WB(0x00, 0x00); // Switch page to 0
        WB(0x0b, 0x00); // Disable VSNS_TX
        WB(0x0c, 0x00); // Disable ISNS_TX
        WB(0x00, 0xfd);
        WB(0x0d, 0x0d);
        WB(0x12, 0x00);
        WB(0x46, 0x1f);
        WB(0x32, 0x28);
        WB(0x00, 0x00);
        WB(0x04, 0xc6);
        WB(0x3e, 0x10);
        WB(0x00, 0x00); // Switch page to 0
    }
    WdfWaitLockRelease(pDevice->StartLock);

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnD0Exit(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
) {
    UNREFERENCED_PARAMETER(FxPreviousState);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    WA(0x00, 0x00); // Switch page to 0
    WA(0x02, 0x01); // Set mode to mute

    if (pDevice->TwoSpeakers) {
        WB(0x00, 0x00); // Switch page to 0
        WB(0x02, 0x01); // Set mode to mute
    }

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

int CsAudioArg2 = 1;

VOID
CSAudioRegisterEndpoint(
    PDEVICE_CONTEXT pDevice
) {
    CsAudioArg arg;
    RtlZeroMemory(&arg, sizeof(CsAudioArg));
    arg.argSz = sizeof(CsAudioArg);
    arg.endpointType = CSAudioEndpointTypeSpeaker;
    arg.endpointRequest = CSAudioEndpointRegister;
    ExNotifyCallback(pDevice->CSAudioAPICallback, &arg, &CsAudioArg2);
}

VOID
CsAudioCallbackFunction(
    IN PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
) {
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");
    if (!pDevice) {
        return;
    }

    if (Argument2 == &CsAudioArg2) {
        return;
    }

    pDevice->CSAudioManaged = TRUE;

    CsAudioArg localArg;
    RtlZeroMemory(&localArg, sizeof(CsAudioArg));
    RtlCopyMemory(&localArg, arg, min(arg->argSz, sizeof(CsAudioArg)));

    if (localArg.endpointType == CSAudioEndpointTypeDSP && localArg.endpointRequest == CSAudioEndpointRegister) {
        CSAudioRegisterEndpoint(pDevice);
    }
    else if (localArg.endpointType != CSAudioEndpointTypeSpeaker) {
        return;
    }

    WdfWaitLockAcquire(pDevice->StartLock, NULL);

    if (localArg.endpointRequest == CSAudioEndpointStop) {
        WA(0x00, 0x00); // Switch page to 0
        WA(0x02, 0x01); // Set mode to mute

        if (pDevice->TwoSpeakers) {
            WB(0x00, 0x00); // Switch page to 0
            WB(0x02, 0x01); // Set mode to mute
        }
    }
    if (localArg.endpointRequest == CSAudioEndpointStart && CheckMuteStatus(pDevice)) {
        WA(0x00, 0x00); // Switch page to 0
        WA(0x02, 0x00); // Set active mode

        if (pDevice->TwoSpeakers) {
            WB(0x00, 0x00); // Switch page to 0
            WB(0x02, 0x00); // Set active mode
        }
    }
    WdfWaitLockRelease(pDevice->StartLock);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return;
}

NTSTATUS
OnSelfManagedIoInit(
    _In_
    WDFDEVICE FxDevice
) {
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

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

    status = ExCreateCallback(&pDevice->CSAudioAPICallback, &attributes, TRUE, TRUE);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    pDevice->CSAudioAPICallbackObj = ExRegisterCallback(pDevice->CSAudioAPICallback,
        CsAudioCallbackFunction,
        pDevice
    );

    if (!pDevice->CSAudioAPICallbackObj) {
        return STATUS_NO_CALLBACK_ACTIVE;
    }

    CSAudioRegisterEndpoint(pDevice);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");


    return status;
}

NTSTATUS
Tas2562CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
    pnpCallbacks.EvtDeviceSelfManagedIoInit = OnSelfManagedIoInit;
    pnpCallbacks.EvtDevicePrepareHardware = OnPrepareHardware;
    pnpCallbacks.EvtDeviceReleaseHardware = OnReleaseHardware;
    pnpCallbacks.EvtDeviceD0Entry = OnD0Entry;
    pnpCallbacks.EvtDeviceD0Exit = OnD0Exit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    WDF_DEVICE_STATE deviceState;
    WDF_DEVICE_STATE_INIT(&deviceState);

    deviceState.NotDisableable = WdfFalse;
    WdfDeviceSetDeviceState(device, &deviceState);

    deviceContext = DeviceGetContext(device);

    status = WdfWaitLockCreate(
        WDF_NO_OBJECT_ATTRIBUTES,
        &deviceContext->StartLock);

    return status;
}

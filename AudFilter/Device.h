EXTERN_C_START

typedef struct _DEVICE_CONTEXT
{
    PCALLBACK_OBJECT        CSAudioAPICallback;
    PVOID                   CSAudioAPICallbackObj;
    BOOLEAN                 CSAudioManaged;
    BOOLEAN                 TIMER_STATUS;
    WDFTIMER                Timer;
    UINT64                  CallCount;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

NTSTATUS
AudFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

VOID
EvtIoStop(
    WDFQUEUE Queue,
    WDFREQUEST Request,
    ULONG ActionFlags
);

VOID
EvtIoResume(
    WDFQUEUE Queue,
    WDFREQUEST Request
);

VOID
OnRequestCompletionRoutine(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
);

VOID
OnDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
);

VOID
FilterForwardRequest(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target
);

typedef enum {
    CSAudioEndpointTypeDSP,
    CSAudioEndpointTypeSpeaker,
    CSAudioEndpointTypeHeadphone,
    CSAudioEndpointTypeMicArray,
    CSAudioEndpointTypeMicJack
} CSAudioEndpointType;

typedef enum {
    CSAudioEndpointRegister,
    CSAudioEndpointStart,
    CSAudioEndpointStop,
    CSAudioEndpointOverrideFormat
} CSAudioEndpointRequest;

typedef struct CSAUDIOFORMATOVERRIDE {
    UINT16 channels;
    UINT16 frequency;
    UINT16 bitsPerSample;
    UINT16 validBitsPerSample;
    BOOLEAN force32BitOutputContainer;
} CsAudioFormatOverride;

typedef struct CSAUDIOARG {
    UINT32 argSz;
    CSAudioEndpointType endpointType;
    CSAudioEndpointRequest endpointRequest;
    union {
        CsAudioFormatOverride formatOverride;
    }CsUnion;
} CsAudioArg, * PCsAudioArg;

VOID
CsAudioCallbackFunction(
    PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
);

EXTERN_C_END

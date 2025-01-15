#include "public.h"

EXTERN_C_START

typedef struct _SPB_CONTEXT
{
    WDFIOTARGET SpbIoTarget;
    LARGE_INTEGER I2cResHubId;
    WDFMEMORY WriteMemory;
    WDFMEMORY ReadMemory;
    WDFWAITLOCK SpbLock;
    WDFREQUEST SpbRequest;
    WDFMEMORY InputMemory;
} SPB_CONTEXT, * PSPB_CONTEXT;

typedef struct _DEVICE_CONTEXT
{
    SPB_CONTEXT             SpbContextA;
    INT                     SpbContextA_ID;
    SPB_CONTEXT             SpbContextB;
    INT                     SpbContextB_ID;
    BOOLEAN                 TwoSpeakers;
    PCALLBACK_OBJECT        CSAudioAPICallback;
    PVOID                   CSAudioAPICallbackObj;
    BOOLEAN                 CSAudioManaged;
    WDFWAITLOCK             StartLock;
} DEVICE_CONTEXT, * PDEVICE_CONTEXT;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)

#define DELAY_MS(msec)  {                                                   \
    LARGE_INTEGER interval;                                              \
    interval.QuadPart = DELAY_ONE_MILLISECOND;                           \
    interval.QuadPart *= msec;                                           \
    KeDelayExecutionThread(KernelMode, 0, &interval);                    \
}

NTSTATUS
Tas2562CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

EXTERN_C_END
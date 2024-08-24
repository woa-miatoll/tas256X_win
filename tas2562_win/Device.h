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
    SPB_CONTEXT             SpbContextB;
    BOOLEAN                 TwoSpeakers;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

NTSTATUS
Tas2562CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EXTERN_C_END

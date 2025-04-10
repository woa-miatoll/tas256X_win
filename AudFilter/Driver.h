#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>

#include "device.h"
#include "trace.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD AudFilterEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP AudFilterEvtDriverContextCleanup;

EXTERN_C_END

#include "driver.h"
#include "spb.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, Tas2562CreateDevice)
#endif

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
        status = STATUS_NOT_FOUND;
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
    UNREFERENCED_PARAMETER(FxDevice);
    UNREFERENCED_PARAMETER(FxPreviousState);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    WA(0x00, 0x00); // Switch page to 0
    WA(0x01, 0x00); // Reset
    WA(0x02, 0x0c); // Set active mode, Disable VSNS_PD, ISNS_PD
    WA(0x03, 0x20); // Set AMP Level to 16dBV
    WA(0x06, 0x09); // Set sampler rate to 48khz
    WA(0x07, 0x02); // Set rx offset to 1
    WA(0x00, 0x02); // Switch page to 2
    WA(0x0c, 0x40); // Volume
    WA(0x0d, 0x40); // Volume
    WA(0x0e, 0x40); // Volume
    WA(0x0f, 0x00); // Volume
    WA(0x00, 0x00); // Switch page to 0
    WA(0x0b, 0x00); // Disable VSNS_TX
    WA(0x0c, 0x00); // Disable ISNS_TX

    if (pDevice->TwoSpeakers) {
        WB(0x00, 0x00); // Switch page to 0
        WB(0x01, 0x00); // Reset
        WB(0x02, 0x0c); // Set mode to active mode, Disable VSNS_PD, ISNS_PD
        WB(0x03, 0x20); // Set AMP Level to 16dBV
        WB(0x06, 0x09); // Set sampler rate to 48khz
        WB(0x07, 0x02); // Set rx offset to 1
        WB(0x00, 0x02); // Switch page to 2
        WB(0x0c, 0x40); // Volume
        WB(0x0d, 0x40); // Volume
        WB(0x0e, 0x40); // Volume
        WB(0x0f, 0x00); // Volume
        WB(0x00, 0x00); // Switch page to 0
        WB(0x0b, 0x00); // Disable VSNS_TX
        WB(0x0c, 0x00); // Disable ISNS_TX
    }

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

    WA(0x02, 0x0e); // Set mode to mute, Disable VSNS_PD, ISNS_PD

    if (pDevice->TwoSpeakers) {
        WB(0x02, 0x0e); // Set mode to mute, Disable VSNS_PD, ISNS_PD
    }

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
Tas2562CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
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

    return status;
}

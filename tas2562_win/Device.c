#include "driver.h"
#include "spb.h"
#include "device.tmh"

#include "tas256x.h"
#include "tas2562.h"
#include "tas2564.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, Tas2562CreateDevice)
#endif

UINT8 p_icn_threshold[] = { 0x64, 0x00, 0x01, 0x2f, 0x2c };
UINT8 p_icn_hysteresis[] = { 0x6c, 0x00, 0x01, 0x5d, 0xc0 };
UINT8 p_2562_dvc[] = { 0x0c, 0x25, 0x25, 0x00, 0x00 };
UINT8 p_2564_dvc[] = { 0x0c, 0x10, 0x10, 0x00, 0x00 };
UINT8 HPF_reverse_path[] = { 0x70, 0x7F, 0xFF,  0xFF,  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

NTSTATUS tas256x_change_book_page(SPB_CONTEXT* SpbContext, UINT32 book, UINT32 page)
{
    NTSTATUS status;
    UINT32 buf[2];

    buf[0] = TAS256X_BOOKCTL_PAGE;
    buf[1] = page;
    status = SpbDeviceWrite(SpbContext, buf, sizeof(buf));

    buf[0] = TAS256X_BOOKCTL_REG;
    buf[1] = book;
    status = SpbDeviceWrite(SpbContext, buf, sizeof(buf));

    return status;
}

NTSTATUS tas256x_reg_write(SPB_CONTEXT* SpbContext, UINT32 reg, UINT8 data)
{
    NTSTATUS status;
    UINT8 buf[2];

    status = tas256x_change_book_page(SpbContext, TAS256X_BOOK_ID(reg), TAS256X_PAGE_ID(reg));

    if (!NT_SUCCESS(status)) {
        return status;
    }

    buf[0] = (UINT8)(reg & 0xFF);
    buf[1] = data;

    return SpbDeviceWrite(SpbContext, buf, sizeof(buf));
}

NTSTATUS tas256x_reg_bulk_write(
    SPB_CONTEXT* SpbContext,
    UINT32 reg,
    UINT8* data,
    UINT8 length
) {
    NTSTATUS status;
    status = tas256x_change_book_page(SpbContext, TAS256X_BOOK_ID(reg), TAS256X_PAGE_ID(reg));

    return SpbDeviceWrite(SpbContext, data, length);
}

NTSTATUS tas256x_reg_read(
    _In_ SPB_CONTEXT* SpbContext,
    UINT32 reg,
    UINT8* data
) {
    NTSTATUS status;

    UINT8 raw_data = 0;
    status = SpbDeviceWriteRead(SpbContext, &reg, &raw_data, sizeof(UCHAR), sizeof(UINT8));
    *data = raw_data;
    return status;
}

NTSTATUS tas256x_reg_update_bits(
    SPB_CONTEXT* SpbContext, 
    UINT32 reg,
    UINT8 mask,
    UINT8 val)
{
    NTSTATUS status;
    UINT8 temp_val, data;

    status = tas256x_reg_read(SpbContext, reg, &data);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    temp_val = data & ~mask;
    temp_val |= val & mask;

    if (data == temp_val)
    {
        status = STATUS_SUCCESS;
        return status;
    }
    status = tas256x_reg_write(SpbContext, reg, temp_val);

    return status;
}


NTSTATUS tas256x_set_power_up(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA, 
        TAS256X_POWERCONTROL,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_ACTIVE);
    
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB, 
            TAS256X_POWERCONTROL,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_ACTIVE);

    /* Let Power on the device */
    DELAY_MS(20);

    /* Enable Comparator Hysteresis */
    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_MISC_CLASSD,
        TAS256X_CMP_HYST_MASK,
        (0x01 << TAS256X_CMP_HYST_SHIFT));
    
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_MISC_CLASSD,
            TAS256X_CMP_HYST_MASK,
            (0x01 << TAS256X_CMP_HYST_SHIFT));

    return n_result;
}

NTSTATUS tas256x_set_power_mute(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    /*Disable Comparator Hysteresis before Power Mute */
    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_MISC_CLASSD,
        TAS256X_CMP_HYST_MASK,
        (0x00 << TAS256X_CMP_HYST_SHIFT));
    
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_MISC_CLASSD,
            TAS256X_CMP_HYST_MASK,
            (0x00 << TAS256X_CMP_HYST_SHIFT));

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA, 
        TAS256X_POWERCONTROL,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_MUTE);
    
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB, 
            TAS256X_POWERCONTROL,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_MUTE);

    return n_result;
}

NTSTATUS tas256x_set_power_shutdown(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    /*Disable Comparator Hysteresis before Power Down */
    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_MISC_CLASSD,
        TAS256X_CMP_HYST_MASK,
        (0x00 << TAS256X_CMP_HYST_SHIFT));

    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_MISC_CLASSD,
            TAS256X_CMP_HYST_MASK,
            (0x00 << TAS256X_CMP_HYST_SHIFT));

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA, TAS256X_POWERCONTROL,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_SHUTDOWN);

    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB, TAS256X_POWERCONTROL,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
            TAS256X_POWERCONTROL_OPERATIONALMODE10_SHUTDOWN);

    /*Device Shutdown need 20ms after shutdown writes are made*/
    DELAY_MS(20);
    return n_result;
}

/*
*  Return true if TAS is active
*  Return false if TAS is muted or shut down
*  Note that we're deviating from downstream driver here.
*/
NTSTATUS tas256x_power_check(SPB_CONTEXT* SpbContext)
{
    int n_result = 0;
    int status = 0;

    n_result = tas256x_reg_update_bits(SpbContext, 
        TAS256X_POWERCONTROL,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK,
        TAS256X_POWERCONTROL_OPERATIONALMODE10_SHUTDOWN);

    status &= TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK;

    if ((status != TAS256X_POWERCONTROL_OPERATIONALMODE10_SHUTDOWN)
        && (status != TAS256X_POWERCONTROL_OPERATIONALMODE10_MUTE))
        return false;
    else
        return true;

}

/*
 *  IV Sense Power Up = 1
 *	IV Sense Power Down = 0
 */
NTSTATUS tas256x_iv_sense_enable_set(PDEVICE_CONTEXT pDevice, bool enable)
{
    NTSTATUS n_result = 0;

    if (enable) {
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_POWERCONTROL,
            TAS256X_POWERCONTROL_ISNSPOWER_MASK |
            TAS256X_POWERCONTROL_VSNSPOWER_MASK,
            TAS256X_POWERCONTROL_VSNSPOWER_ACTIVE |
            TAS256X_POWERCONTROL_ISNSPOWER_ACTIVE);
        
        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_POWERCONTROL,
                TAS256X_POWERCONTROL_ISNSPOWER_MASK |
                TAS256X_POWERCONTROL_VSNSPOWER_MASK,
                TAS256X_POWERCONTROL_VSNSPOWER_ACTIVE |
                TAS256X_POWERCONTROL_ISNSPOWER_ACTIVE);
    }
    else {
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_POWERCONTROL,
            TAS256X_POWERCONTROL_ISNSPOWER_MASK |
            TAS256X_POWERCONTROL_VSNSPOWER_MASK,
            TAS256X_POWERCONTROL_VSNSPOWER_ACTIVE |
            TAS256X_POWERCONTROL_ISNSPOWER_ACTIVE);
        
        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_POWERCONTROL,
                TAS256X_POWERCONTROL_ISNSPOWER_MASK |
                TAS256X_POWERCONTROL_VSNSPOWER_MASK,
                TAS256X_POWERCONTROL_VSNSPOWER_ACTIVE |
                TAS256X_POWERCONTROL_ISNSPOWER_ACTIVE);
    }
    return n_result;
}

NTSTATUS tas256x_iv_slot_config(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG5, 0xff, 0x46);

    tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG6, 0xff, 0x44);

    if (pDevice->TwoSpeakers) {
        tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG5, 0xff, 0x42);

        tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG6, 0xff, 0x40);
    }

    return n_result;
}

NTSTATUS tas256x_iv_bitwidth_config(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG2,
        TAS256X_TDMCONFIGURATIONREG2_IVLEN76_MASK,
        TAS256X_TDMCONFIGURATIONREG2_IVLENCFG76_16BITS);

    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG6,
            TAS256X_TDMCONFIGURATIONREG2_IVLEN76_MASK,
            TAS256X_TDMCONFIGURATIONREG2_IVLENCFG76_16BITS);

    return n_result;
}

NTSTATUS tas256x_set_samplerate(PDEVICE_CONTEXT pDevice, int samplerate)
{
    NTSTATUS n_result = 0;
    UINT8 data = 0;
    UINT8 data2 = 0;

    switch (samplerate) {
    case 48000:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_48KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_44_1_48KHZ;
        break;
    case 44100:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_44_1KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_44_1_48KHZ;
        break;
    case 96000:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_48KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_88_2_96KHZ;
        break;
    case 88200:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_44_1KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_88_2_96KHZ;
        break;
    case 19200:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_48KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_176_4_192KHZ;
        break;
    case 17640:
        data = TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_44_1KHZ;
        data2 = TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_176_4_192KHZ;
        break;
    }

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG0,
        TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_MASK,
        data);

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG0,
        TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_MASK,
        data2);

    if (pDevice->TwoSpeakers) {
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG0,
            TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_MASK,
            data);

        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG0,
            TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_MASK,
            data2);
    }

    return n_result;
}

/*
 *rx_edge = 0; Falling
 *= 1; Rising
 */
NTSTATUS tas256x_rx_set_fmt(PDEVICE_CONTEXT pDevice, unsigned int rx_edge, UINT8 rx_start_slot)
{
    NTSTATUS n_result = 0;
    UINT8 data = 0;

    if (rx_edge)
        data = TAS256X_TDMCONFIGURATIONREG1_RXEDGE_RISING;
    else
        data = TAS256X_TDMCONFIGURATIONREG1_RXEDGE_FALLING;

    tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG1,
        TAS256X_TDMCONFIGURATIONREG1_RXEDGE_MASK,
        data);

    tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_TDMCONFIGURATIONREG1,
        TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_MASK,
        (rx_start_slot <<
            TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_SHIFT));
    
    if (pDevice->TwoSpeakers)
        tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG1,
            TAS256X_TDMCONFIGURATIONREG1_RXEDGE_MASK,
            data);

        tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_TDMCONFIGURATIONREG1,
            TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_MASK,
            (rx_start_slot <<
                TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_SHIFT));


    return n_result;
}

NTSTATUS tas256x_rx_set_slot(PDEVICE_CONTEXT pDevice, int slot_width)
{
    NTSTATUS n_result = -1;

    switch (slot_width) {
    case 16:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_16BITS);

        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_16BITS);
        break;
    case 24:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_24BITS);

        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_24BITS);
        break;
    case 32:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_32BITS);

        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_32BITS);
        break;
    }

    return n_result;
}

NTSTATUS tas256x_rx_set_bitwidth(PDEVICE_CONTEXT pDevice, int bitwidth)
{
    NTSTATUS n_result = 0;
    switch (bitwidth) {
    case 16:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_16BITS);
        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_16BITS);
        break;
    case 24:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_24BITS);
        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_24BITS);
        break;
    case 32:
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
            TAS256X_TDMCONFIGURATIONREG2,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
            TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_32BITS);
        if (pDevice->TwoSpeakers)
            n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
                TAS256X_TDMCONFIGURATIONREG2,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK,
                TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_32BITS);
        break;
    }

    return n_result;
}

NTSTATUS tas256x_update_rx_cfg(SPB_CONTEXT* SpbContext, int value)
{
    NTSTATUS n_result = 0;
    UINT8 data = 0;

    switch (value) {
    case 0:
        data = TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_I2C;
        break;
    case 1:
        data = TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_LEFT;
        break;
    case 2:
        data = TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_RIGHT;
        break;
    case 3:
        data = TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_STEREO_DOWNMIX;
        break;
    }

    n_result = tas256x_reg_update_bits(SpbContext,
        TAS256X_TDMCONFIGURATIONREG2,
        TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MASK,
        data);

    return n_result;
}

NTSTATUS tas256x_software_reset(SPB_CONTEXT* SpbContext)
{
    NTSTATUS n_result;

    n_result = tas256x_reg_write(SpbContext,
        TAS256X_SOFTWARERESET,
        TAS256X_SOFTWARERESET_SOFTWARERESET_RESET);
        
   DELAY_MS(20);

   return n_result;
}

NTSTATUS tas256x_boost_volt_update(SPB_CONTEXT* SpbContext, int value)
{
    NTSTATUS n_result = 0;

    if (value == 2564)
        n_result = tas256x_reg_write(SpbContext,
            TAS256X_PLAYBACKCONFIGURATIONREG0,
            TAS2564_AMP_LEVEL_16dBV);
    if (value == 2562)
        n_result = tas256x_reg_write(SpbContext,
            TAS256X_PLAYBACKCONFIGURATIONREG0,
            0x20);

    return n_result;
}

NTSTATUS tas256x_set_misc_config(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result = 0;

    n_result = tas256x_reg_write(&pDevice->SpbContextA,
        TAS256X_MISCCONFIGURATIONREG0,
        0xcf);

    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_write(&pDevice->SpbContextB,
            TAS256X_MISCCONFIGURATIONREG0,
            0xcf);

    return n_result;
}

NTSTATUS tas256x_set_clock_config(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result;

    n_result = tas256x_reg_write(&pDevice->SpbContextA,
        TAS256X_CLOCKCONFIGURATION,
        0x0c);

    n_result |= tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_INTERRUPTCONFIGURATION,
        TAS256X_CLOCK_HALT_TIMER_MASK,
        TAS256X_CLOCK_HALT_838MS);
    
    if (pDevice->TwoSpeakers) {
        n_result = tas256x_reg_write(&pDevice->SpbContextB,
            TAS256X_CLOCKCONFIGURATION,
            0x0c);

        n_result |= tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_INTERRUPTCONFIGURATION,
            TAS256X_CLOCK_HALT_TIMER_MASK,
            TAS256X_CLOCK_HALT_838MS);
    }

    return n_result;
}

NTSTATUS tas256x_interrupt_clear(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result;

    n_result = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_INTERRUPTCONFIGURATION,
        TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR_MASK,
        TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR);
    if (pDevice->TwoSpeakers) {
        n_result = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_INTERRUPTCONFIGURATION,
            TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR_MASK,
            TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR);
    }

    return n_result;
}

NTSTATUS tas256x_interrupt_enable(PDEVICE_CONTEXT pDevice, int val)
{
    NTSTATUS n_result;

    if (val) {
        n_result = tas256x_reg_write(&pDevice->SpbContextA,
            TAS256X_INTERRUPTMASKREG0,
            0xf8);
        n_result = tas256x_reg_write(&pDevice->SpbContextA,
            TAS256X_INTERRUPTMASKREG1,
            0xb1);
        if (pDevice->TwoSpeakers) {
            n_result = tas256x_reg_write(&pDevice->SpbContextB,
                TAS256X_INTERRUPTMASKREG0,
                0xf8);
            n_result = tas256x_reg_write(&pDevice->SpbContextB,
                TAS256X_INTERRUPTMASKREG1,
                0xb1);
        }
    }
    else {
        n_result = tas256x_reg_write(&pDevice->SpbContextA,
            TAS256X_INTERRUPTMASKREG0,
            TAS256X_INTERRUPTMASKREG0_DISABLE);
        n_result = tas256x_reg_write(&pDevice->SpbContextA,
            TAS256X_INTERRUPTMASKREG1,
            TAS256X_INTERRUPTMASKREG1_DISABLE);
        if (pDevice->TwoSpeakers) {
            n_result = tas256x_reg_write(&pDevice->SpbContextB,
                TAS256X_INTERRUPTMASKREG0,
                TAS256X_INTERRUPTMASKREG0_DISABLE);
            n_result = tas256x_reg_write(&pDevice->SpbContextB,
                TAS256X_INTERRUPTMASKREG1,
                TAS256X_INTERRUPTMASKREG1_DISABLE);
        }
    }

    return n_result;
}

NTSTATUS tas256x_icn_data(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS n_result;

    n_result = tas256x_reg_bulk_write(&pDevice->SpbContextA, TAS256X_ICN_THRESHOLD_REG, p_icn_threshold, sizeof(p_icn_threshold));
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_bulk_write(&pDevice->SpbContextB, TAS256X_ICN_THRESHOLD_REG, p_icn_threshold, sizeof(p_icn_threshold));

    n_result = tas256x_reg_bulk_write(&pDevice->SpbContextA, TAS256X_ICN_HYSTERESIS_REG, p_icn_hysteresis, sizeof(p_icn_hysteresis));
    if (pDevice->TwoSpeakers)
        n_result = tas256x_reg_bulk_write(&pDevice->SpbContextB, TAS256X_ICN_HYSTERESIS_REG, p_icn_hysteresis, sizeof(p_icn_hysteresis));

    return n_result;
}

/*
* Note that we're deviating from downstream driver here.
*/
NTSTATUS tas256x_update_playback_volume(SPB_CONTEXT* SpbContext, int value)
{
    NTSTATUS status = 0;

    switch (value) {
    case 2562:
        status = tas256x_reg_bulk_write(SpbContext, TAS256X_DVC_PCM, p_2562_dvc, sizeof(p_2562_dvc));
        break;
    case 2564:
        status = tas256x_reg_bulk_write(SpbContext, TAS256X_DVC_PCM, p_2564_dvc, sizeof(p_2564_dvc));
        break;
    }

    return status;
}

NTSTATUS tas256x_set_power_state(PDEVICE_CONTEXT pDevice, int state)
{
    NTSTATUS n_result = 0;

    switch (state) {
    case TAS256X_POWER_ACTIVE:
        tas256x_rx_set_fmt(pDevice, 0, 1); // Set rx offset to 1, set rx clock polarity to falling edge
        tas256x_iv_sense_enable_set(pDevice, 1);
        tas256x_interrupt_clear(pDevice);
        tas256x_interrupt_enable(pDevice, 0);
        tas256x_set_power_up(pDevice);
        tas256x_icn_data(pDevice);
        break;
    case TAS256X_POWER_MUTE:
        tas256x_set_power_mute(pDevice);
        tas256x_interrupt_enable(pDevice, 0);
        break;
    case TAS256X_POWER_SHUTDOWN:
        tas256x_interrupt_enable(pDevice, 0);
        tas256x_set_power_shutdown(pDevice);
        tas256x_iv_sense_enable_set(pDevice, 0);
        break;
    }

    return n_result;
}

NTSTATUS tas2564_rx_mode_update(SPB_CONTEXT* SpbContext, int rx_mode)
{
    NTSTATUS n_result = 0;

    if (rx_mode) {
        n_result = tas256x_reg_update_bits(SpbContext,
            TAS2564_PLAYBACKCONFIGURATIONREG0,
            TAS2564_PLAYBACKCONFIGURATIONREG_RX_SPKR_MODE_MASK,
            TAS2564_PLAYBACKCONFIGURATIONREG_RX_MODE);
        n_result = tas256x_reg_write(SpbContext,
            TAS256X_TEST_PAGE_LOCK,
            0xd);
        n_result |= tas256x_reg_write(SpbContext,
            TAS256X_DAC_MODULATOR,
            0xc0);
        n_result |= tas256x_reg_write(SpbContext,
            TAS256X_ICN_IMPROVE,
            0x1f);
        n_result |= tas256x_reg_write(SpbContext,
            TAS256X_CLASSDCONFIGURATION2,
            0x08);
        n_result |= tas256x_reg_write(SpbContext,
            TAS256X_MISCCONFIGURATIONREG0,
            0xca);
        n_result |= tas256x_reg_write(SpbContext,
            TAS256X_ICN_SW_REG,
            0x00);
    }

    return n_result;
}

NTSTATUS tas2564_specific(SPB_CONTEXT* SpbContext)
{
    NTSTATUS n_result;

    n_result = tas256x_boost_volt_update(SpbContext, 2564);
    n_result = tas2564_rx_mode_update(SpbContext, 1);
    
    return n_result;
}

NTSTATUS tas2562_specific(SPB_CONTEXT* SpbContext)
{
    NTSTATUS n_result;

    n_result = tas256x_boost_volt_update(SpbContext, 2562);

    return n_result;
}

NTSTATUS tas256x_set_tx_config(PDEVICE_CONTEXT pDevice) {
    NTSTATUS status;

    status = tas256x_reg_write(&pDevice->SpbContextA,
        TAS256X_TDMConfigurationReg4, 0x11);
    if (pDevice->TwoSpeakers)
        status = tas256x_reg_write(&pDevice->SpbContextB,
            TAS256X_TDMConfigurationReg4, 0xf1);
    
    return status;
}

NTSTATUS tas256x_icn_config(PDEVICE_CONTEXT pDevice) {
    NTSTATUS status;

    status = tas256x_reg_write(&pDevice->SpbContextA,
        TAS256X_REG(0, 253, 13), 0x0d);
    status = tas256x_reg_write(&pDevice->SpbContextA,
        TAS256X_REG(0, 253, 25), 0x80);
    if (pDevice->TwoSpeakers) {
        status = tas256x_reg_write(&pDevice->SpbContextB,
            TAS256X_REG(0, 253, 13), 0x0d);
        status = tas256x_reg_write(&pDevice->SpbContextB,
            TAS256X_REG(0, 253, 25), 0x80);
    }

    return status;
}
NTSTATUS tas256x_load_ctrl_values(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS status = 0;

    status = tas256x_update_playback_volume(&pDevice->SpbContextA, 2562);
    status = tas256x_update_playback_volume(&pDevice->SpbContextB, 2564);
    status = tas256x_update_rx_cfg(&pDevice->SpbContextA, 1);
    status = tas256x_update_rx_cfg(&pDevice->SpbContextB, 2);

    return status;
}

NTSTATUS tas256x_HPF_FF_Bypass(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS status = 0;

    status = tas256x_reg_update_bits(&pDevice->SpbContextA,
        TAS256X_PLAYBACKCONFIGURATIONREG0,
        TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_MASK,
        TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_DISABLED);
    if (pDevice->TwoSpeakers)
        status = tas256x_reg_update_bits(&pDevice->SpbContextB,
            TAS256X_PLAYBACKCONFIGURATIONREG0,
            TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_MASK,
            TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_DISABLED);

    return status;
}

NTSTATUS tas256x_HPF_FB_Bypass(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS status = 0;

    status = tas256x_reg_bulk_write(&pDevice->SpbContextA,
        TAS256X_HPF, HPF_reverse_path, sizeof(HPF_reverse_path) / sizeof(HPF_reverse_path[0]));
    if (pDevice->TwoSpeakers)
        status = tas256x_reg_bulk_write(&pDevice->SpbContextB,
            TAS256X_HPF, HPF_reverse_path, sizeof(HPF_reverse_path) / sizeof(HPF_reverse_path[0]));

    return status;
}

/*
* Note that we're deviating from downstream driver here.
*/
NTSTATUS tas256x_set_bitwidth_slotwidth(PDEVICE_CONTEXT pDevice, int bitwidth, int slotwidth)
{
    NTSTATUS status = 0;

    status = tas256x_rx_set_bitwidth(pDevice, bitwidth);
    status = tas256x_rx_set_slot(pDevice, slotwidth);
    status = tas256x_iv_slot_config(pDevice);
    status = tas256x_iv_bitwidth_config(pDevice);

    return status;
}

NTSTATUS tas256x_load_init(PDEVICE_CONTEXT pDevice)
{
    NTSTATUS status = 0;

    status = tas256x_set_misc_config(pDevice); // Disable OTE/OCE retry, enable IRQZ pull up
    status = tas256x_set_tx_config(pDevice);
    status = tas256x_set_clock_config(pDevice);
    status = tas256x_icn_config(pDevice);
    status = tas256x_HPF_FF_Bypass(pDevice);
    status = tas256x_HPF_FB_Bypass(pDevice);

    return status;
}

VOID
tas256x_load_config(
    PDEVICE_CONTEXT pDevice
) {
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Enter!");
    WdfWaitLockAcquire(pDevice->StartLock, NULL);

    tas256x_software_reset(&pDevice->SpbContextA);
    tas256x_software_reset(&pDevice->SpbContextB);
    tas256x_iv_slot_config(pDevice);
    tas256x_load_init(pDevice);
    tas2564_specific(&pDevice->SpbContextB);
    tas2562_specific(&pDevice->SpbContextA);
    tas256x_load_ctrl_values(pDevice);
    tas256x_rx_set_fmt(pDevice, 0, 1);
    tas256x_set_bitwidth_slotwidth(pDevice, 24, 16);
    tas256x_set_samplerate(pDevice, 48000);
    tas256x_set_power_state(pDevice, 0);

    WdfWaitLockRelease(pDevice->StartLock);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
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
    
    tas256x_load_config(pDevice);

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

    tas256x_set_power_state(pDevice, 2);

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

    if (localArg.endpointRequest == CSAudioEndpointStop) {
        tas256x_set_power_state(pDevice, 2);
    }
    if (localArg.endpointRequest == CSAudioEndpointStart && !(tas256x_power_check(&pDevice->SpbContextA) || tas256x_power_check(&pDevice->SpbContextB))) {
        tas256x_load_config(pDevice);
    }
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

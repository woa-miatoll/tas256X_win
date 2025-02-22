#pragma once

#define CODEC_PM

#define bool int

#define true 1
#define false 0

/* Page Control Register */
#define TAS256X_PAGECTL_REG			0

/* Book Control Register (available in page0 of each book) */
#define TAS256X_BOOKCTL_PAGE		0
#define TAS256X_BOOKCTL_REG			127

#define TAS256X_REG(book, page, reg)	(((book * 256 * 128) + (page * 128)) + reg)

#define TAS256X_BOOK_ID(reg)			(reg / (256 * 128))

#define TAS256X_PAGE_ID(reg)			((reg % (256 * 128)) / 128)

#define TAS256X_BOOK_REG(reg)			(reg % (256 * 128))

#define TAS256X_PAGE_REG(reg)			((reg % (256 * 128)) % 128)

/* Software Reset */
#define TAS256X_SOFTWARERESET  TAS256X_REG(0x0, 0x0, 0x01)
#define TAS256X_SOFTWARERESET_SOFTWARERESET_MASK  (0x1 << 0)
#define TAS256X_SOFTWARERESET_SOFTWARERESET_DONTRESET  (0x0 << 0)
#define TAS256X_SOFTWARERESET_SOFTWARERESET_RESET  (0x1 << 0)

/* Power Control */
#define TAS256X_POWERCONTROL  TAS256X_REG(0x0, 0x0, 0x02)
#define TAS256X_POWERCONTROL_ISNSPOWER_MASK  (0x1 << 3)
#define TAS256X_POWERCONTROL_ISNSPOWER_ACTIVE  (0x0 << 3)
#define TAS256X_POWERCONTROL_ISNSPOWER_POWEREDDOWN  (0x1 << 3)
#define TAS256X_POWERCONTROL_VSNSPOWER_MASK  (0x1 << 2)
#define TAS256X_POWERCONTROL_VSNSPOWER_ACTIVE  (0x0 << 2)
#define TAS256X_POWERCONTROL_VSNSPOWER_POWEREDDOWN  (0x1 << 2)
#define TAS256X_POWERCONTROL_OPERATIONALMODE10_MASK  (0x3 << 0)
#define TAS256X_POWERCONTROL_OPERATIONALMODE10_ACTIVE  (0x0 << 0)
#define TAS256X_POWERCONTROL_OPERATIONALMODE10_MUTE  (0x1 << 0)
#define TAS256X_POWERCONTROL_OPERATIONALMODE10_SHUTDOWN  (0x2 << 0)

/* Playback Configuration Reg0 */
#define TAS256X_PLAYBACKCONFIGURATIONREG0  TAS256X_REG(0x0, 0x0, 0x03)
#define TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_MASK  (0x1 << 6)
#define TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_ENABLED  (0x0 << 6)
#define TAS256X_PLAYBACKCONFIGURATIONREG0_DCBLOCKER_DISABLED  (0x1 << 6)

/* Misc Configuration Reg0 */
#define TAS256X_MISCCONFIGURATIONREG0  TAS256X_REG(0x0, 0x0, 0x04)
#define TAS256X_MISCCONFIGURATIONREG0_CPPGRETRY_MASK  (0x1 << 7)
#define TAS256X_MISCCONFIGURATIONREG0_CPPGRETRY_DONOTRETRY  (0x0 << 7)
#define TAS256X_MISCCONFIGURATIONREG0_CPPGRETRY_RETRY  (0x1 << 7)
#define TAS256X_MISCCONFIGURATIONREG0_VBATPRORETRY_MASK  (0x1 << 6)
#define TAS256X_MISCCONFIGURATIONREG0_VBATPRORETRY_DONOTRETRY  (0x0 << 6)
#define TAS256X_MISCCONFIGURATIONREG0_VBATPRORETRY_RETRY  (0x1 << 6)
#define TAS256X_MISCCONFIGURATIONREG0_OCERETRY_MASK  (0x1 << 5)
#define TAS256X_MISCCONFIGURATIONREG0_OCERETRY_DONOTRETRY  (0x0 << 5)
#define TAS256X_MISCCONFIGURATIONREG0_OCERETRY_RETRY  (0x1 << 5)
#define TAS256X_MISCCONFIGURATIONREG0_OTERETRY_MASK  (0x1 << 4)
#define TAS256X_MISCCONFIGURATIONREG0_OTERETRY_DONOTRETRY  (0x0 << 4)
#define TAS256X_MISCCONFIGURATIONREG0_OTERETRY_RETRY  (0x1 << 4)
#define TAS256X_MISCCONFIGURATIONREG0_IRQZPULL_MASK  (0x1 << 3)
#define TAS256X_MISCCONFIGURATIONREG0_IRQZPULL_DISABLED  (0x0 << 3)
#define TAS256X_MISCCONFIGURATIONREG0_IRQZPULL_ENABLED  (0x1 << 3)
#define TAS256X_MISCCONFIGURATIONREG0_AMPSS_MASK  (0x1 << 2)
#define TAS256X_MISCCONFIGURATIONREG0_AMPSS_DISABLED  (0x0 << 2)
#define TAS256X_MISCCONFIGURATIONREG0_AMPSS_ENABLED  (0x1 << 2)

/* TDM Configuration Reg0 */
#define TAS256X_TDMCONFIGURATIONREG0  TAS256X_REG(0x0, 0x0, 0x06)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_MASK  (0x1 << 5)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_48KHZ  (0x0 << 5)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATERAMP_44_1KHZ  (0x1 << 5)
#define TAS256X_TDMCONFIGURATIONREG0_DETECTSAMPRATE_MASK  (0x1 << 4)
#define TAS256X_TDMCONFIGURATIONREG0_DETECTSAMPRATE_DISABLED  (0x1 << 4)
#define TAS256X_TDMCONFIGURATIONREG0_DETECTSAMPRATE_ENABLED  (0x0 << 4)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_MASK  (0x7 << 1)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_14_7_16KHZ  (0x1 << 1)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_44_1_48KHZ  (0x4 << 1)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_88_2_96KHZ  (0x5 << 1)
#define TAS256X_TDMCONFIGURATIONREG0_SAMPRATE31_176_4_192KHZ  (0x6 << 1)
#define TAS256X_TDMCONFIGURATIONREG0_FRAMESTART_MASK  (0x1 << 0)
#define TAS256X_TDMCONFIGURATIONREG0_FRAMESTART_LOWTOHIGH  (0x0 << 0)
#define TAS256X_TDMCONFIGURATIONREG0_FRAMESTART_HIGHTOLOW  (0x1 << 0)

/* TDM Configuration Reg1 */
#define TAS256X_TDMCONFIGURATIONREG1  TAS256X_REG(0x0, 0x0, 0x07)
#define TAS256X_TDMCONFIGURATIONREG1_RXJUSTIFY_MASK  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG1_RXJUSTIFY_LEFT  (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG1_RXJUSTIFY_RIGHT  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_MASK  (0x1f << 1)
#define TAS256X_TDMCONFIGURATIONREG1_RXOFFSET51_SHIFT (1)
#define TAS256X_TDMCONFIGURATIONREG1_RXEDGE_MASK  (0x1 << 0)
#define TAS256X_TDMCONFIGURATIONREG1_RXEDGE_RISING  (0x0 << 0)
#define TAS256X_TDMCONFIGURATIONREG1_RXEDGE_FALLING  (0x1 << 0)

/* TDM Configuration Reg2 */
#define TAS256X_TDMCONFIGURATIONREG2  TAS256X_REG(0x0, 0x0, 0x08)
#define TAS256X_TDMCONFIGURATIONREG2_IVLEN76_MASK (0X3 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVLENCFG76_16BITS (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVLENCFG76_12BITS (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVLENCFG76_8BITS (0x2 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVMONLEN76_MASK  (0x3 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVMONLEN76_8BITS  (0x2 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVMONLEN76_12BITS  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_IVMONLEN76_16BITS  (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MASK  (0x3 << 4)
#define TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_I2C  (0x0 << 4)
#define TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_LEFT  (0x1 << 4)
#define TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_MONO_RIGHT  (0x2 << 4)
#define TAS256X_TDMCONFIGURATIONREG2_RXSCFG54_STEREO_DOWNMIX  (0x3 << 4)
#define TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_MASK  (0x3 << 2)
#define TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_16BITS  (0x0 << 2)
#define TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_20BITS  (0x1 << 2)
#define TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_24BITS  (0x2 << 2)
#define TAS256X_TDMCONFIGURATIONREG2_RXWLEN32_32BITS  (0x3 << 2)
#define TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_MASK  (0x3 << 0)
#define TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_16BITS  (0x0 << 0)
#define TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_24BITS  (0x1 << 0)
#define TAS256X_TDMCONFIGURATIONREG2_RXSLEN10_32BITS  (0x2 << 0)

/* TDM Configuration Reg4 */
#define TAS256X_TDMConfigurationReg4  TAS256X_REG(0x0, 0x0, 0x0A)
#define TAS256X_TDMCONFIGURATIONREG4_TXKEEPER_MASK  (0x1 << 5)
#define TAS256X_TDMCONFIGURATIONREG4_TXKEEPER_DISABLE  (0x0 << 5)
#define TAS256X_TDMCONFIGURATIONREG4_TXKEEPER_ENABLE  (0x1 << 5)
#define TAS256X_TDMCONFIGURATIONREG4_TXFILL_MASK  (0x1 << 4)
#define TAS256X_TDMCONFIGURATIONREG4_TXFILL_TRANSMIT0  (0x0 << 4)
#define TAS256X_TDMCONFIGURATIONREG4_TXFILL_TRANSMITHIZ  (0x1 << 4)
#define TAS256X_TDMCONFIGURATIONREG4_TXOFFSET31_MASK  (0x7 << 1)
#define TAS256X_TDMCONFIGURATIONREG4_TXEDGE_MASK  (0x1 << 0)
#define TAS256X_TDMCONFIGURATIONREG4_TXEDGE_RISING  (0x0 << 0)
#define TAS256X_TDMCONFIGURATIONREG4_TXEDGE_FALLING  (0x1 << 0)

/* TDM Configuration Reg5 */
#define TAS256X_TDMCONFIGURATIONREG5  TAS256X_REG(0x0, 0x0, 0x0B)
#define TAS256X_TDMCONFIGURATIONREG5_VSNSTX_MASK  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG5_VSNSTX_DISABLE  (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG5_VSNSTX_ENABLE  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG5_VSNSSLOT50_MASK  (0x3f << 0)

/* TDM Configuration Reg6 */
#define TAS256X_TDMCONFIGURATIONREG6  TAS256X_REG(0x0, 0x0, 0x0C)
#define TAS256X_TDMCONFIGURATIONREG6_ISNSTX_MASK  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG6_ISNSTX_DISABLE  (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG6_ISNSTX_ENABLE  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG6_ISNSSLOT50_MASK  (0x3f << 0)

/* TDM Configuration Reg7 */
#define TAS256X_TDMCONFIGURATIONREG7  TAS256X_REG(0x0, 0x0, 0x0D)
#define TAS256X_TDMCONFIGURATIONREG7_VBATSLEN_MASK  (0x1 << 7)
#define TAS256X_TDMCONFIGURATIONREG7_VBATSLEN_8BITS  (0x0 << 7)
#define TAS256X_TDMCONFIGURATIONREG7_VBATSLEN_16BITS  (0x1 << 7)
#define TAS256X_TDMCONFIGURATIONREG7_VBATTX_MASK  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG7_VBATTX_DISABLE  (0x0 << 6)
#define TAS256X_TDMCONFIGURATIONREG7_VBATTX_ENABLE  (0x1 << 6)
#define TAS256X_TDMCONFIGURATIONREG7_VBATSLOT50_MASK  (0x3f << 0)

/* Interrupt Mask Reg0 */
#define TAS256X_INTERRUPTMASKREG0  TAS256X_REG(0x0, 0x0, 0x1A)
#define TAS256X_INTERRUPTMASKREG0_LIMMUTEINTMASK_MASK  (0x1 << 7)
#define TAS256X_INTERRUPTMASKREG0_LIMMUTEINTMASK_UNMASK  (0x0 << 7)
#define TAS256X_INTERRUPTMASKREG0_LIMMUTEINTMASK_DISABLE  (0x1 << 7)
#define TAS256X_INTERRUPTMASKREG0_LIMINFHLDINTMASK_MASK  (0x1 << 6)
#define TAS256X_INTERRUPTMASKREG0_LIMINFHLDINTMASK_UNMASK  (0x0 << 6)
#define TAS256X_INTERRUPTMASKREG0_LIMINFHLDINTMASK_DISABLE  (0x1 << 6)
#define TAS256X_INTERRUPTMASKREG0_LIMMAXATNINTMASK_MASK  (0x1 << 5)
#define TAS256X_INTERRUPTMASKREG0_LIMMAXATNINTMASK_UNMASK  (0x0 << 5)
#define TAS256X_INTERRUPTMASKREG0_LIMMAXATNINTMASK_DISABLE  (0x1 << 5)
#define TAS256X_INTERRUPTMASKREG0_VBATLESSTHANINFINTMASK_MASK  (0x1 << 4)
#define TAS256X_INTERRUPTMASKREG0_VBATLESSTHANINFINTMASK_UNMASK  (0x0 << 4)
#define TAS256X_INTERRUPTMASKREG0_VBATLESSTHANINFINTMASK_DISABLE  (0x1 << 4)
#define TAS256X_INTERRUPTMASKREG0_LIMACTIVEFLAGINTMASK_MASK  (0x1 << 3)
#define TAS256X_INTERRUPTMASKREG0_LIMACTIVEFLAGINTMASK_UNMASK  (0x0 << 3)
#define TAS256X_INTERRUPTMASKREG0_LIMACTIVEFLAGINTMASK_DISABLE  (0x1 << 3)
#define TAS256X_INTERRUPTMASKREG0_TDMCLOCKERRORINTMASK_MASK  (0x1 << 2)
#define TAS256X_INTERRUPTMASKREG0_TDMCLOCKERRORINTMASK_UNMASK  (0x0 << 2)
#define TAS256X_INTERRUPTMASKREG0_TDMCLOCKERRORINTMASK_DISABLE  (0x1 << 2)
#define TAS256X_INTERRUPTMASKREG0_OCEINTMASK_MASK  (0x1 << 1)
#define TAS256X_INTERRUPTMASKREG0_OCEINTMASK_UNMASK  (0x0 << 1)
#define TAS256X_INTERRUPTMASKREG0_OCEINTMASK_DISABLE  (0x1 << 1)
#define TAS256X_INTERRUPTMASKREG0_OTEINTMASK_MASK  (0x1 << 0)
#define TAS256X_INTERRUPTMASKREG0_OTEINTMASK_UNMASK  (0x0 << 0)
#define TAS256X_INTERRUPTMASKREG0_OTEINTMASK_DISABLE  (0x1 << 0)
#define TAS256X_INTERRUPTMASKREG0_DISABLE 0xff

/* Interrupt Mask Reg1 */
#define TAS256X_INTERRUPTMASKREG1  TAS256X_REG(0x0, 0x0, 0x1B)
#define TAS256X_INTERRUPTMASKREG1_DSPOUTPUTINTMASK_MASK  (0x1 << 7)
#define TAS256X_INTERRUPTMASKREG1_DSPOUTPUTINTMASK_UNMASK  (0x0 << 7)
#define TAS256X_INTERRUPTMASKREG1_DSPOUTPUTINTMASK_DISABLE  (0x1 << 7)
#define TAS256X_INTERRUPTMASKREG1_CRCINTMASK_MASK  (0x1 << 6)
#define TAS256X_INTERRUPTMASKREG1_CRCINTMASK_UNMASK  (0x0 << 6)
#define TAS256X_INTERRUPTMASKREG1_CRCINTMASK_DISABLE  (0x1 << 6)
#define TAS256X_INTERRUPTMASKREG1_VBATOVLOINTMASK_MASK  (0x1 << 2)
#define TAS256X_INTERRUPTMASKREG1_VBATOVLOINTMASK_UNMASK  (0x0 << 2)
#define TAS256X_INTERRUPTMASKREG1_VBATOVLOINTMASK_DISABLE  (0x1 << 2)
#define TAS256X_INTERRUPTMASKREG1_VBATUVLOINTMASK_MASK  (0x1 << 1)
#define TAS256X_INTERRUPTMASKREG1_VBATUVLOINTMASK_UNMASK  (0x0 << 1)
#define TAS256X_INTERRUPTMASKREG1_VBATUVLOINTMASK_DISABLE  (0x1 << 1)
#define TAS256X_INTERRUPTMASKREG1_BROWNOUTFLAGINTMASK_MASK  (0x1 << 0)
#define TAS256X_INTERRUPTMASKREG1_BROWNOUTFLAGINTMASK_UNMASK  (0x0 << 0)
#define TAS256X_INTERRUPTMASKREG1_BROWNOUTFLAGINTMASK_DISABLE  (0x1 << 0)
#define TAS256X_INTERRUPTMASKREG1_DISABLE 0xff

/* Latched-Interrupt Reg0 */
#define TAS256X_LATCHEDINTERRUPTREG0  TAS256X_REG(0x0, 0x0, 0x24)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMUTESTICKY_MASK  (0x1 << 7)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMUTESTICKY_NOINTERRUPT  (0x0 << 7)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMUTESTICKY_INTERRUPT  (0x1 << 7)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMINFHLDSTICKY_MASK  (0x1 << 6)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMINFHLDSTICKY_NOINTERRUPT  (0x0 << 6)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMINFHLDSTICKY_INTERRUPT  (0x1 << 6)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMAXATNSTICKY_MASK  (0x1 << 5)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMAXATNSTICKY_NOINTERRUPT  (0x0 << 5)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMMAXATNSTICKY_INTERRUPT  (0x1 << 5)
#define TAS256X_LATCHEDINTERRUPTREG0_VBATLESSTHANINFSTICKY_MASK  (0x1 << 4)
#define TAS256X_LATCHEDINTERRUPTREG0_VBATLESSTHANINFSTICKY_NOINTERRUPT (0x0 << 4)
#define TAS256X_LATCHEDINTERRUPTREG0_VBATLESSTHANINFSTICKY_INTERRUPT  (0x1 << 4)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMACTIVEFLAGSTICKY_MASK  (0x1 << 3)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMACTIVEFLAGSTICKY_NOINTERRUPT  (0x0 << 3)
#define TAS256X_LATCHEDINTERRUPTREG0_LIMACTIVEFLAGSTICKY_INTERRUPT  (0x1 << 3)
#define TAS256X_LATCHEDINTERRUPTREG0_TDMCLOCKERRORSTICKY_MASK  (0x1 << 2)
#define TAS256X_LATCHEDINTERRUPTREG0_TDMCLOCKERRORSTICKY_NOINTERRUPT  (0x0 << 2)
#define TAS256X_LATCHEDINTERRUPTREG0_TDMCLOCKERRORSTICKY_INTERRUPT  (0x1 << 2)
#define TAS256X_LATCHEDINTERRUPTREG0_OCEFLAGSTICKY_MASK  (0x1 << 1)
#define TAS256X_LATCHEDINTERRUPTREG0_OCEFLAGSTICKY_NOINTERRUPT  (0x0 << 1)
#define TAS256X_LATCHEDINTERRUPTREG0_OCEFLAGSTICKY_INTERRUPT  (0x1 << 1)
#define TAS256X_LATCHEDINTERRUPTREG0_OTEFLAGSTICKY_MASK  (0x1 << 0)
#define TAS256X_LATCHEDINTERRUPTREG0_OTEFLAGSTICKY_NOINTERRUPT  (0x0 << 0)
#define TAS256X_LATCHEDINTERRUPTREG0_OTEFLAGSTICKY_INTERRUPT  (0x1 << 0)

/* Latched-Interrupt Reg1 */
#define TAS256X_LATCHEDINTERRUPTREG1  TAS256X_REG(0x0, 0x0, 0x25)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMAUDDATAINVALIDSTICKY_MASK  (0x1 << 7)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMAUDDATAINVALIDSTICKY_NOINTERRUPT (0x0 << 7)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMAUDDATAINVALIDSTICKY_INTERRUPT (0x1 << 7)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATOVLOSTICKY_MASK  (0x1 << 3)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATOVLOSTICKY_NOINTERRUPT  (0x0 << 3)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATOVLOSTICKY_INTERRUPT  (0x1 << 3)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATUVLOSTICKY_MASK  (0x1 << 2)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATUVLOSTICKY_NOINTERRUPT  (0x0 << 2)
#define TAS256X_LATCHEDINTERRUPTREG1_VBATUVLOSTICKY_INTERRUPT  (0x1 << 2)
#define TAS256X_LATCHEDINTERRUPTREG1_BROWNOUTFLAGSTICKY_MASK  (0x1 << 1)
#define TAS256X_LATCHEDINTERRUPTREG1_BROWNOUTFLAGSTICKY_NOINTERRUPT  (0x0 << 1)
#define TAS256X_LATCHEDINTERRUPTREG1_BROWNOUTFLAGSTICKY_INTERRUPT  (0x1 << 1)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMCLOCKERRORSTICKY_MASK  (0x1 << 0)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMCLOCKERRORSTICKY_NOINTERRUPT  (0x0 << 0)
#define TAS256X_LATCHEDINTERRUPTREG1_PDMCLOCKERRORSTICKY_INTERRUPT  (0x1 << 0)

/* Interrupt Configuration */
#define TAS256X_INTERRUPTCONFIGURATION  TAS256X_REG(0x0, 0x0, 0x30)
#define TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR_MASK (0x1 << 2)
#define TAS256X_INTERRUPTCONFIGURATION_LTCHINTCLEAR (0x1 << 2)
#define TAS256X_INTERRUPTCONFIGURATION_PININTCONFIG10_MASK  (0x3 << 0)
#define TAS256X_INTERRUPTCONFIGURATION_PININTCONFIG10_ASSERTONLIVEINTERRUPTS (0x0 << 0)
#define TAS256X_INTERRUPTCONFIGURATION_PININTCONFIG10_ASSERTONLATCHEDINTERRUPTS (0x1 << 0)
#define TAS256X_INTERRUPTCONFIGURATION_PININTCONFIG10_ASSERT2MSONLIVEINTERRUPTS (0x2 << 0)
#define TAS256X_INTERRUPTCONFIGURATION_PININTCONFIG10_ASSERT2MSONLATCHEDINTERRUPTS (0x3 << 0)

#define TAS256X_BOOSTSLOPE TAS256X_REG(0x0, 0x0, 0x35)
#define TAS256X_CLOCK_HALT_TIMER_MASK	(0x7 << 3)
#define TAS256X_CLOCK_HALT_1MS		(0x0 << 3)
#define TAS256X_CLOCK_HALT_3MS		(0x1 << 3)
#define TAS256X_CLOCK_HALT_26MS		(0x2 << 3)
#define TAS256X_CLOCK_HALT_52MS		(0x3 << 3)
#define TAS256X_CLOCK_HALT_104MS	(0x4 << 3)
#define TAS256X_CLOCK_HALT_209MS	(0x5 << 3)
#define TAS256X_CLOCK_HALT_419MS	(0x6 << 3)
#define TAS256X_CLOCK_HALT_838MS	(0x7 << 3)
#define TAS256X_BOOSTSLOPE_MASK		(0x3 << 2)

/* Clock Configuration */
#define TAS256X_CLOCKCONFIGURATION  TAS256X_REG(0x0, 0x0, 0x38)
#define TAS256X_CLOCKCONFIGURATION_SBCLKTOFS52_MASK  (0xf << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_16  (0x0 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_24  (0x1 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_32  (0x2 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_48  (0x3 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_64  (0x4 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_96  (0x5 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_128  (0x6 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_192  (0x7 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_256  (0x8 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_384  (0x9 << 2)
#define TAS256X_CLOCKCONFIGURATION_SBCLKtoFS52_512  (0xa << 2)
#define TAS256X_CLOCKCONFIGURATION_DISCLKRATEDETECT10_MASK  (0x3 << 0)
#define TAS256X_CLOCKCONFIGURATION_DISCLKRATEDETECT10_DISABLED  (0x1 << 0)
#define TAS256X_CLOCKCONFIGURATION_DISCLKRATEDETECT10_ENABLED  (0x0 << 0)

#define TAS256X_VBATFILTER TAS256X_REG(0x0, 0x0, 0x3b)
#define TAS256X_VBAT_LPF_MASK	(0x3 << 5)
#define TAS256X_VBAT_LPF_SHIFT	5

#define TAS256X_CLASSHRELEASETIMER TAS256X_REG(0x0, 0x0, 0x3c)

#define TAS256X_BOOST_CFG4 TAS256X_REG(0x0, 0x0, 0x40)

#define TAS256X_CHIPID TAS256X_REG(0x0, 0x0, 0x7D)

#define TAS256X_ICN_THRESHOLD_REG TAS256X_REG(0x0, 0x2, 0x64)
#define TAS256X_ICN_HYSTERESIS_REG TAS256X_REG(0x0, 0x2, 0x6c)

#define TAS256X_ICN_SW_REG TAS256X_REG(0x0, 0x0, 0x3e)
#define TAS256X_ICN_SW_MASK (0x01 << 4)
#define TAS256X_ICN_SW_ENABLE  (0x10)
#define TAS256X_ICN_SW_DISABLE  (0x00)
#define TAS256X_TESTPAGECONFIGURATION TAS256X_REG(0x0, 0xfd, 0xd)
#define TAS256X_CLASSDCONFIGURATION1	TAS256X_REG(0x0, 0xfd, 0x19)
#define TAS256X_CLASSDCONFIGURATION2	TAS256X_REG(0x0, 0xfd, 0x32)
#define TAS256X_CLASSDCONFIGURATION3	TAS256X_REG(0x0, 0xfd, 0x33)
#define TAS256X_CLASSDCONFIGURATION4	TAS256X_REG(0x0, 0xfd, 0x3f)
#define TAS256X_EFFICIENCYCONFIGURATION	TAS256X_REG(0x0, 0xfd, 0x5f)

#define TAS256X_HPF TAS256X_REG(0x0, 0x2, 0x70)
#define TAS256X_CLASSHHEADROOM TAS256X_REG(0x64, 0x7, 0x48)
#define TAS256X_CLASSHHYSTERESIS TAS256X_REG(0x64, 0x7, 0x4c)
#define TAS256X_CLASSHMTCT TAS256X_REG(0x64, 0x5, 0x4c)

/*PCM Digital Volume Control*/
#define TAS256X_DVC_PCM  TAS256X_REG(0x0, 0x2, 0x0c)

/*Limiter Controls*/
#define TAS256X_LIM_MAX_ATN	TAS256X_REG(0x0, 0x2, 0x14)
#define TAS256X_LIMB_TH_MAX	TAS256X_REG(0x0, 0x2, 0x18)
#define TAS256X_LIMB_TH_MIN	TAS256X_REG(0x0, 0x2, 0x1c)
#define TAS256X_LIMB_INF_PT	TAS256X_REG(0x0, 0x2, 0x20)
#define TAS256X_LIMB_SLOPE	TAS256X_REG(0x0, 0x2, 0x24)

/*Brown Out Protection Control*/
#define TAS256X_BOP_TH	TAS256X_REG(0x0, 0x2, 0x28)
#define TAS256X_BOSD_TH	TAS256X_REG(0x0, 0x2, 0x2c)

#define TAS256X_BOOSTCONFIGURATION4 TAS256X_REG(0x0, 0x0, 0x40)
#define TAS256X_BOOSTCONFIGURATION4_BOOSTCURRENTLIMIT_MASK  (0x3f << 0)
#define TAS256X_BOOSTCONFIGURATION4_BOOSTCURRENTLIMIT_SHIFT  0

#define TAS256X_LIMITERCONFIGURATION0	TAS256X_REG(0x0, 0x0, 0x12)
#define TAS256X_LIMITER_ENABLE_MASK	(0x1 << 0)
#define TAS256X_LIMITER_ENABLE_SHIFT	0x0
#define TAS256X_LIMITER_ATTACKRATE_MASK	(0x7 << 1)
#define TAS256X_LIMITER_ATTACKRATE_SHIFT	0x1
#define TAS256X_LIMITER_ATTACKSTEPSIZE_MASK	(0x3 << 4)
#define TAS256X_LIMITER_ATTACKSTEPSIZE_SHIFT	0x4

#define TAS256X_LIMITERCONFIGURATION1	TAS256X_REG(0x0, 0x0, 0x13)
#define TAS256X_LIMITER_RELEASERATE_MASK	(0x7 << 3)
#define TAS256X_LIMITER_RELEASERATE_SHIFT	0x3
#define TAS256X_LIMITER_RELEASESTEPSIZE_MASK	(0x3 << 6)
#define TAS256X_LIMITER_RELEASESTEPSIZE_SHIFT	0x6

#define TAS256X_BOPCONFIGURATION0	TAS256X_REG(0x0, 0x0, 0x14)
#define TAS256X_BOP_ENABLE_MASK	(0x1 << 0)
#define TAS256X_BOP_ENABLE_SHIFT	0x0
#define TAS256X_BOP_MUTE_MASK	(0x1 << 1)
#define TAS256X_BOP_MUTE_SHIFT	0x1
#define TAS256X_BOP_SHUTDOWN_ENABLE_MASK	(0x1 << 4)
#define TAS256X_BOP_SHUTDOWN_ENABLE_SHIFT	0x4

#define TAS256X_BOPCONFIGURATION1	TAS256X_REG(0x0, 0x0, 0x15)
#define TAS256X_BOP_ATTACKRATE_MASK	(0x7 << 5)
#define TAS256X_BOP_ATTACKRATE_SHIFT	0x5
#define TAS256X_BOP_ATTACKSTEPSIZE_MASK	(0x3 << 0x3)
#define TAS256X_BOP_ATTACKSTEPSIZE_SHIFT	0x3
#define TAS256X_BOP_HOLDTIME_MASK	(0x7 << 0x0)
#define TAS256X_BOP_HOLDTIME_SHIFT	0x0

/*Test Page*/
#define TAS256X_TEST_PAGE_LOCK  TAS256X_REG(0x00, 0xfd, 0x0d)
#define TAS256X_DAC_MODULATOR  TAS256X_REG(0x00, 0xfd, 0x12)
#define TAS256X_DAC_MUTE  TAS256X_REG(0x00, 0xfd, 0x47)
#define TAS256X_ICN_MODE  TAS256X_REG(0x00, 0xfd, 0x64)
#define TAS256X_ICN_IMPROVE  TAS256X_REG(0x00, 0xfd, 0x46)

#define TAS256X_MISC_CLASSD  TAS256X_REG(0x0, 0x01, 0x21)
#define TAS256X_CMP_HYST_MASK  (0x1 << 3)
#define TAS256X_CMP_HYST_SHIFT  (0x3)

/* Book */
#define TAS256X_BOOK  TAS256X_REG(0x0, 0x0, 0x7F)
#define TAS256X_BOOK_BOOK70_MASK  (0xff << 0)

/* data format */
#define TAS256X_DATAFORMAT_SHIFT		2
#define TAS256X_DATAFORMAT_I2S			0x0
#define TAS256X_DATAFORMAT_DSP			0x1
#define TAS256X_DATAFORMAT_RIGHT_J		0x2
#define TAS256X_DATAFORMAT_LEFT_J		0x3

#define TAS256X_DAI_FMT_MASK		(0x7 << TAS256X_DATAFORMAT_SHIFT)

#define TAS256X_POWER_ACTIVE 0
#define TAS256X_POWER_MUTE 1
#define TAS256X_POWER_SHUTDOWN 2

#define ERROR_NONE		0x0000000
#define ERROR_PLL_ABSENT	0x0000001
#define ERROR_DEVA_I2C_COMM	0x0000002
#define ERROR_DEVB_I2C_COMM	0x0000004
#define ERROR_CLOCK			0x0000008
#define ERROR_YRAM_CRCCHK	0x0000010
#define ERROR_OVER_CURRENT	0x0000020
#define ERROR_DIE_OVERTEMP	0x0000040
#define ERROR_OVER_VOLTAGE	0x0000080
#define ERROR_UNDER_VOLTAGE	0x0000100
#define ERROR_BROWNOUT		0x0000200
#define ERROR_CLASSD_PWR	0x0000400
#define ERROR_FAILSAFE      0x0000800

#define CHECK_PERIOD	5000	/* 5 second */

#define TAS256X_I2C_RETRY_COUNT      3
#define ERROR_I2C_SUSPEND           -1
#define ERROR_I2C_FAILED            -2

#define TAS256X_LEFT_SWITCH		0x10000001
#define TAS256X_RIGHT_SWITCH	0x10000002
#define RX_SCFG_LEFT			0x10000003
#define RX_SCFG_RIGHT			0x10000004

#define RESTART_MAX 3

#define tas256x_dvc_pcm 55
#define tas256x_lim_max_attn 7
#define tas256x_lim_thr_max 13
#define tas256x_lim_thr_min 3
#define tas256x_lim_infl_pt 13
#define tas256x_lim_trk_slp 0
#define tas256x_bop_thd 4
#define tas256x_bosd_thd 2
#define tas256x_lim_switch 0
#define tas256x_lim_att_rate 1
#define tas256x_lim_att_stp_size 1
#define tas256x_lim_rel_rate 6
#define tas256x_lim_rel_stp_size 1

#define tas256x_vbat_lpf 2

#define tas2562_bst_vltg 0xA
#define tas2562_bst_ilm 0x36
#define tas2562_bop_enable 1
#define tas2562_bop_mute 0
#define tas2562_bosd_enable 0
#define tas2562_bop_att_rate 1
#define tas2562_bop_att_stp_size 1
#define tas2562_bop_hld_time 6

#define tas2564_rx_mode 0
#define tas2564_bst_vltg 13
#define tas2564_bst_ilm 0x39
#define tas2564_bop_enable 1
#define tas2564_bop_mute 0
#define tas2564_bosd_enable 0
#define tas2564_bop_att_rate 1
#define tas2564_bop_att_stp_size 1
#define tas2564_bop_hld_time 6

typedef enum
{
	TAS2562_CHIP1 = 0x10,
	TAS2562_CHIP2 = 0x20,
	TAS2564_CHIP = 0x00,
}TAS256X_CHIP;
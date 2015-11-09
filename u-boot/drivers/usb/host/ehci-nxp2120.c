/*
 * swpark@nexell.co.kr add for nxp2120 ehci host driver
 */
#include <common.h>
#include <usb.h>
#include <platform.h>

#include "ehci.h"
#include "ehci-core.h"

#define msleep(ms)			wait_ms((ms)*2)

static bool usb_hc_init = false;

static int set_host_mode(void)
{
	u_int UDC_BASE = IO_ADDRESS(PHY_BASEADDR_UDC);
	u_int UDC_MODE = IO_ADDRESS(0xC0018848);

	/* CLOCK_GEN : clock div=0, 3:use ext_clk */
	*(volatile unsigned int *)(UDC_BASE + 0x8C4) = (
													(0<<15) |		// clk output enabled
													(0<< 5) |		// clock divide is 1
													(3<< 2) |		// ext clock
													(0<< 1)			// nomal clock output(falling edge)
													);


	/* CLOCK_ENB : clock always, clk enable */
	*(volatile unsigned int *)(UDC_BASE + 0x8C0) = (
													(1<<3) |	// PCLK Op mode is always
													(1<<2) |	// Clock Generation Enable
													(3<<0)		// usbd clock mode is always
													);

	/* PHY power disable */
	*(volatile unsigned short *)(UDC_BASE + 0x052) = (1<<0);
	//*(volatile unsigned short *)(UDC_BASE + 0x052) = (0<<0);

	*(volatile unsigned short *)(UDC_BASE + 0x840) = (
													(0<<9) |	// XO bias, and PLL blocks remain powered in suspend mode.
													(0<<7) |	// Ref Clk Freq is 12MHz
													(1<<5) |	// XO block uses an external clock supplied on the XO pin
													(1<<4) |	// POR Enable
													(0<<3) |	// POR
													(1<<2) |	// suspendm enable
													(1<<1) |	// suspend normal op mode
													(1<<0)		// ext VBUS enable
													);			// 12MHz,

	*(volatile unsigned short *)(UDC_BASE + 0x052) = (
													(1<<0) |
													(1<<7)
													);			// phy block off, utmi reset

	/* Dummy delay : required over 10 usec */
	udelay(10);

	*(volatile unsigned short *)(UDC_BASE + 0x052) = 0;			// phy block on

	*(volatile unsigned short *)(UDC_BASE + 0x848) = (
													(0<<9) |	// pull-down resistance on D+ is disabled
													(0<<8) |	// pull-down resistance on D- is disabled
													(0<<7) |	// this bit actived if opmode is 2b11
													(0<<6) |	// this bit actived if opmode is 2b11
													(1<<5) |	// OTG block is powered down
													(1<<4) |	// Ext VBus is enable
													(1<<3) |	// VBus valid comparator is enabled.
													(0<<2) |	// disabled charging VBus through the  SRP pull-up resistance
													(0<<1) |	// disable dischrging VBus through the SRP pull-down resistance
													(0<<0)		// ID pin sampling is disbled, and IDDDIG output is not valid.
													);

	/* set usb host mode */
	*(volatile unsigned int *)(UDC_MODE) = (*(volatile unsigned int *)(UDC_MODE) | (1<<10));

	return 0;
}

static void start_usb_hc(void) 
{
	u_int HC_BASE = IO_ADDRESS(PHY_BASEADDR_EHCI);

	if(true == usb_hc_init)
		return;

	/* set host mode */
	set_host_mode();

	/* set host module */
	*(volatile unsigned int *)(HC_BASE + 0x3C4) =
											0<<15 |	// Specifies the direction of the PADVCLK pad. You have to set this bit when CLKSRCSEL0/1 is 3 or 4. 0 : Enable(Output)                    1 : Reserved
											0<<5 |	// Specifies divider value of the source clock.
													//	Divider value = CLKDIV0 + 1
													//	Clock Divisor. For divide - by - N, enter an [N - 1] value.
													//	000000 ~ 111111(N-1) : Divide Value (N) = Divide by 1 to 64
													//	Ex) For divide-by-2 : [0001b]
											3<<2 |	// Clock Source Selection
													//	000 : Reserved			001 : Resrved
													//	010 : Reserved			011 : Ext Clock
													//	100 ~ 111 : Reserved
											0<<1;	// Specifies whether to invert the clock output.
													// 0 : Normal (Falling Edge)	1 : Invert (Rising Edge)


	*(volatile unsigned int *)(HC_BASE + 0x3C0) =
											1<<3 |	// PCLK Operating Mode 0 : Clock is enabled only when CPU accesses 1 : Always
											1<<2 |	// Clock Generation Enable 0 : Disable	1 : Enable
											3<<0;	// USBD Clock Enable
													// 00 : Disable		01 : Reserved		10 : Dynamic	11 : Always
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C0) =
											0<<9 |	// Common Block Power-Down Control :
														//	Function: This signal controls the power-down signals in the XO, Bias, and PLL blocks when the USB 2.0 nanoPHY is suspended.
														//	o 1: The XO, Bias, and PLL blocks are powered down in Suspend mode.
														//	o 0: The XO, Bias, and PLL blocks remain powered in Suspend mode.
											0<<7 |	// Reference Clock Frequency Select
													//	Function: This bus selects the USB 2.0 nanoPHY reference clock frequency.
													//	o 11: Reserved
													//	o 10: 48 MHz
													//	o 01: 24 MHz
													//	o 00: 12 MHz
											1<<5 |	// Reference Clock Select for PLL Block
													//	Function: This signal selects the reference clock source for the PLL block.
													//	o 11: The PLL uses CLKCORE as reference.
													//	o 10: The PLL uses CLKCORE as reference.
													//	o 01: The XO block uses an external clock supplied on the XO pin.
													//	o 00: The XO block uses the clock from a crystal.
											0<<4 |	// POR Enbale
													// 0 : Disable	1: Enable
											0<<3 |	// Power-On Reset
													// Function: This customer-specific signal resets all test registers and state machines in the USB 2.0 nanoPHY.
													// The POR signal must be asserted for a minimum of 10 ¥ìs.
											0<<2 |	// SuspendM Enbale
													// 0 : Disable	1: Enable
											0<<1;	// Suspend Assertion
													//	Function: Asserting this signal suspends the USB 2.0 nanoPHY by tristating the transmitter and powering down the USB 2.0 nanoPHY circuits.
													//	o 1: Normal operating mode
													//	o 0: Suspend mode
													//	USB 2.0 nanoPHY power-down behavior can be overridden using the test interface.
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C4) = 0;		// VBUS Interrupt Enbale
													// 0 : Disable	1: Enable
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C8) = 0;		// VBUS Interrupt Pending Enbale
													// 0 : Disable	1: Enable
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0CC) =
											0<<9 |	// TermSel Enbale
													// 0 : Disable	1: Enable
											0<<8 |	// USB Termination Select
													//	Function: This controller signal sets FS or HS termination.
													//	o 1: Full-speed termination is enabled.
													//	o 0: High-speed termination is enabled.
											1<<7 |	// WordInterface Enbale
													// 0 : Disable	1: Enable
											1<<6 |	// UTMI Data Bus Width and Clock Select
													//	Function: This controller signal selects the data bus width of the UTMI data input and output paths.
													//	o 1: 16-bit data interface
													//	o 0: 8-bit data interface
													//	The USB 2.0 nanoPHY supports 8/16-bit data interfaces for all valid USB 2.0 nanoPHY speed modes.
											0<<5 |	// OPMode Enbale
													// 0 : Disable	1: Enable
											0<<3 |	// UTMI Operational Mode
													//	Function: This controller bus selects the UTMI operational mode.
													//	o 11: Normal operation without SYNC or EOP generation.
													//			If the XCVRSEL bus is not set to 2'b00 when OPMODE[1:0] is set to 2'b11,
													//			USB 2.0 nanoPHY behavior is undefined.
													//	o 10: Disable bit stuffing and NRZI encoding
													//	o 01: Non-driving
													//	o 00: Normal
											0<<2 |	// XCVRSel Enbale
													// 0 : Disable	1: Enable
											0<<0;	// Transceiver Select
													//	Function: This controller bus selects the HS, FS, or LS Transceiver.
													//	o 11: Sends an LS packet on an FS bus or receives an LS packet.
													//	o 10: LS Transceiver
													//	o 01: FS Transceiver
													//	o 00: HS Transceiver
	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D0) = 0x320;	//

	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D4) = 1<<15 | 1<<14 | 1<<13 | 0x20<<7 | 0x20<<1;	// default value

	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D8) = 0x6D78;	// default value

	msleep(1);

	usb_hc_init = true;
}

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(void)
{
	start_usb_hc();

	hccr = (struct ehci_hccr *)(PHY_BASEADDR_EHCI);
	hcor = (struct ehci_hcor *)((uint32_t) hccr
			+ HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	printf("nxp2120 ehci init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)hccr, (uint32_t)hcor,
		(uint32_t)HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
	return 0;
}

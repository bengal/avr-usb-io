/*
 * Simple program to control I/O on a AVR device through USB
 */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>
#include "usbdrv.h"
#include "oddebug.h"
#include "requests.h"

#include "io.h"

/* Read a byte from the input shift registers */
uint8_t shift_in_byte(uint8_t do_load)
{
	uint8_t val = 0;
	uint8_t i;

	/* load parallel data */
	if (do_load) {
		IM_OUT &= ~ _BV(IM_PL_BIT);
		IM_OUT |= _BV(IM_PL_BIT);
	}

	for (i = 0; i < 8; i++) {
		/* shift bit in */
		val = val << 1;
		val |= (IM_IN & _BV(IM_Q7_BIT)) >> IM_Q7_BIT;

		/* pulse clock */
		IM_OUT &= ~_BV(IM_CP_BIT);
		IM_OUT |= _BV(IM_CP_BIT);
	}

	return val;
}

/* Enable a pin of the output multiplexers */
void enable_output(uint8_t pin_num)
{
	uint8_t chip;
	uint8_t addr;

	chip = pin_num >> 4;
	addr = pin_num; /* & 0xF ? */

	/* select output chip */
	OM_OUT |= _BV(OM_E0_BIT);
	OM_OUT |= _BV(OM_E1_BIT);
	OM_OUT |= _BV(OM_E2_BIT);

	if (chip == 0)
		OM_OUT &=  ~_BV(OM_E0_BIT);
	else if (chip == 1)
		OM_OUT &=  ~_BV(OM_E1_BIT);
	else if (chip == 2)
		OM_OUT &=  ~_BV(OM_E2_BIT);

	/* output pin address */
	OM_OUT &= ~_BV(OM_S0_BIT);
	OM_OUT &= ~_BV(OM_S1_BIT);
	OM_OUT &= ~_BV(OM_S2_BIT);
	OM_OUT &= ~_BV(OM_S3_BIT);

	if (addr & _BV(0))
		OM_OUT |= _BV(OM_S0_BIT);
	if (addr & _BV(1))
		OM_OUT |= _BV(OM_S1_BIT);
	if (addr & _BV(2))
		OM_OUT |= _BV(OM_S2_BIT);
	if (addr & _BV(3))
		OM_OUT |= _BV(OM_S3_BIT);
}

void setup_io()
{
	/* set outputs */
	OM_DDR |= _BV(OM_S0_BIT);
	OM_DDR |= _BV(OM_S1_BIT);
	OM_DDR |= _BV(OM_S2_BIT);
	OM_DDR |= _BV(OM_S3_BIT);
	OM_DDR |= _BV(OM_E0_BIT);
	OM_DDR |= _BV(OM_E1_BIT);
	OM_DDR |= _BV(OM_E2_BIT);
	IM_DDR |= _BV(IM_CP_BIT);
	IM_DDR |= _BV(IM_PL_BIT);
	/* set input */
	IM_DDR &= ~_BV(IM_Q7_BIT);

	/* set initial value for PL and CP */
	IM_OUT |= _BV(IM_PL_BIT);
	IM_OUT |= _BV(IM_CP_BIT);
}

/* USB message handler */
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void *)data;
	static uchar dataBuffer[4];

	if (rq->bRequest == REQ_ECHO) {
		dataBuffer[0] = rq->wValue.bytes[0];
		dataBuffer[1] = rq->wValue.bytes[1];
		dataBuffer[2] = rq->wIndex.bytes[0];
		dataBuffer[3] = rq->wIndex.bytes[1];
		usbMsgPtr = dataBuffer;
		return 4;
	} else if (rq->bRequest == REQ_CONFIG) {
		/* Not implemented yet */
	} else if(rq->bRequest == REQ_READ) {
		dataBuffer[0] = shift_in_byte(1);
		dataBuffer[1] = shift_in_byte(0);
		dataBuffer[2] = shift_in_byte(0);
		dataBuffer[3] = shift_in_byte(0);
		usbMsgPtr = dataBuffer;
		return 4;
	} else if(rq->bRequest == REQ_WRITE) {
		enable_output(rq->wValue.bytes[0]);
	}

	return 0;   /* return no data back to host */
}

int __attribute__((noreturn)) main(void)
{
	uchar   i;

	setup_io();

	odDebugInit();
	DBG1(0x00, 0, 0);		/* debug output: main starts */
	usbInit();
	usbDeviceDisconnect();		/* enforce re-enumeration */
	i = 0;
	while (--i) {			/* fake USB disconnect for > 250 ms */
		wdt_reset();
		_delay_ms(1);
	}
	usbDeviceConnect();

	sei();
	DBG1(0x01, 0, 0);		/* debug output: main loop starts */
	for (;;) {			/* main event loop */
		DBG1(0x02, 0, 0);	/* debug output: main loop iterates */
		wdt_reset();
		usbPoll();
	}
}

/***********************************************************************
* Application for AVR UART, TWI, GPIO peripherals, etc.                *
* @author Kevin Harper                                                 *
* @date July 12, 2023                                                  *
* Purpose: Show how to use the GPIO driver in the applications         *
*          Debugged with UART thru the onboard CH340 IC for the nano   *
*		   which is home to an ATMega328P                              *
***********************************************************************/

#include <avr/io.h>
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define F_SCL 400000UL
#define UART_BAUD 115200UL

#include "uart.h"
#include <avr/interrupt.h>
#include "twi.h"
#include "gpio_types.h"
#include "adc/adc.h"
#include "tmr1.h"
#include <stdlib.h>  //itoa()
#include "spi/spi.h"

uint8_t print_buffer[24];
uint16_t* adc_val;
uint8_t error;

void main(void) {

	uart_init(TX, false, NONE);

	_delay_ms(10);

	uart_transmit_string((unsigned char*)"UART configured as output @ 115200 baud");
	uart_transmit_nl(1, false);

	adc_init(ANALOG_VCC, FREE, ADC3, false);

	uart_transmit_string((unsigned char*)"ADC initialized to read from PC6 with analog VCC as reference");
	uart_transmit_nl(1, false);

	gpio_pin_init(GPIO_B, DIR_OUTPUT, PINB2);

	uart_transmit_string((unsigned char*)"Shift register serial data pin PB7 initialized as output");
	uart_transmit_nl(1, false);

	gpio_pin_init(GPIO_B, DIR_OUTPUT, PINB0);

	uart_transmit_string((unsigned char*)"Shift register storage latch pin initialized as output");
	uart_transmit_nl(1, false);

	gpio_pin_write(GPIO_B, PINB0, LOW);
	gpio_pin_write(GPIO_B, PINB2, LOW);

	clock_init(1023);

	uart_transmit_string((unsigned char*)"SRCLK initialized");
	uart_transmit_nl(1, false);

	sei(); /*Enable interrupts, necessary for I2C ANDDDD for PWM or timers*/

	uart_transmit_string((unsigned char*)"Interrupts enabled");
	uart_transmit_nl(1, false);
    
	_delay_ms(1000);

	uart_transmit_string((unsigned char*)"Initialization complete.");
	uart_transmit_nl(2, false);

	while(1) {
		
		gpio_pin_write(GPIO_B, PINB0, LOW);
		gpio_pin_write(GPIO_B, PINB2, HIGH);
		// PINB0 is storage clk
		// PINB2 is serial data

		adc_read(ADC3, adc_val);

		uart_transmit_string((unsigned char*)utoa(*adc_val, print_buffer, 10));
		uart_transmit_nl(2, false);

		uart_transmit_string((unsigned char*)utoa((uint16_t)((*adc_val)*(uint16_t)64 + (uint16_t)1023), print_buffer, 10));
		uart_transmit_nl(2, false);

		// this function misbehaves with numbers greater than 32767
		update_clock(SCALE_COUNTS(*adc_val));

		gpio_pin_write(GPIO_B, PINB0, HIGH);
		gpio_pin_write(GPIO_B, PINB2, LOW);

		//gpio_pin_write(GPIO_B, PINB0, HIGH);
		_delay_ms(100);
	}
}

/* =================================================================================
 *                                    // END FILE //
 * ================================================================================= */
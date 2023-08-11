#ifndef TMR1_H_
#define TMR1_H_

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include "uart.h"

#define PWM_TOP 39999U
// For 1ms pulse
#define SERVO_MIN 1999U
// for 2ms pulse
#define SERVO_MAX 3999U

#define SCALE_COUNTS(x) (uint16_t)((x*(uint16_t)64) + (uint16_t)1023)

void clock_init(uint16_t initial_val);
void update_clock(uint16_t i);
void pwm_init();
void pwm_sweep();
void servo_set(uint16_t deg, uint16_t max_deg);

#endif //TMR1_H_
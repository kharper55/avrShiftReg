#include "tmr1.h"

volatile static uint8_t update_pwm_ready = 0;
volatile static uint8_t update_clk_ready = 0;
uint16_t tmr1_top;

// When this ISR is executed the OCF1A flag is cleared automatically in the TIFR1
ISR(TIMER1_OVF_vect) {
    update_pwm_ready = 0;
}

// for debug purposes... breakpoint setting etc.
ISR(TIMER1_COMPA_vect) {
    update_clk_ready = 0;
}

// For shift register clock; choose frequency between 1 and 120Hz
// The counter value will map from an adc reading 
void clock_init(uint16_t initial_val) {
    tmr1_top = initial_val;
    // Set portb bit 1 to output as it never specifies that the tccr1 peripheral takes
    // explicit control of the port
    DDRB |= (1 << DDB1);
    // Enable output compare match A interrupt
    TIMSK1 = (1 << OCIE1A);

    // Set output compare match counter to initialize at 39999 counts. With PSC of 8, results in 50Hz
    OCR1AH = ((initial_val) & 0xFF00) >> 8;
    OCR1AL = ((initial_val) & 0x00FF);

    // Set TOGGLE on compare match mode on A, disable B output, set CTC mode with OCR1A top setter
    TCCR1A = (0b01 << COM1A0) | (0b00 << COM1B0) | (0b00 << WGM10); 

    // Finalize waveform generation mode and set presecalar (64), enable timer 
    // output compare module
    TCCR1B = (0b01 << WGM12) | (0b011 << CS10);
}

void update_clock(uint16_t i) {
    update_clk_ready = 1;
    while(update_clk_ready != 0) {
        // Update compare match value
        OCR1AH = (i & 0xFF00) >> 8;
        OCR1AL = (i & 0x00FF);
    }
    tmr1_top = i;
}

void pwm_init() {

    // Set portb bit 1 to output as it never specifies that the tccr1 peripheral takes
    // explicit control of the port
    DDRB |= (1 << DDB1);

    // Enable timer overflow interrupt and output compare match A interrupt
    TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);

    // Set top value of input caputre register according to H->L write rule for 16 bit reg
    ICR1H = (PWM_TOP & 0xFF00) >> 8;
    ICR1L = (PWM_TOP & 0x00FF);

    // Set servo to minimum displacement
    OCR1AH = (SERVO_MIN & 0xFF00) >> 8;
    OCR1AL = (SERVO_MIN & 0x00FF);
    
    // Set clear on compare match mode on A, disable B output, set fast PWM mode
    TCCR1A = (0b10 << COM1A0) | (0b00 << COM1B0) | (0b10 << WGM10); 

    // Finalize waveform generation mode and set presecalar (8), enable timer 
    // output compare module
    TCCR1B = (0b11 << WGM12) | (0b010 << CS10);

}

static void update_pwm(uint16_t i) {
    update_pwm_ready = 1;
    while(update_pwm_ready != 0) {
        // Update compare match value
        OCR1AH = (i & 0xFF00) >> 8;
        OCR1AL = (i & 0x00FF);
    }
}

void pwm_sweep() {
    uint16_t i = 0;

    for (i = SERVO_MIN; i <= SERVO_MAX; i+=20) {
        update_pwm(i);
        _delay_ms(40);
    }

    for (i = SERVO_MAX; i >= SERVO_MIN; i-=20) {
        update_pwm(i);
        _delay_ms(40);
    }
}

void servo_set(uint16_t deg, uint16_t max_deg) {

    float set = (float)deg / (float)max_deg;

    // Uses ratio of degrees to find whatever the value should be for the PWM
    set = (((float)SERVO_MAX - (float)SERVO_MIN)*set) + (float)SERVO_MIN;
    uint16_t point = (uint16_t)set;
    update_pwm(point);
}
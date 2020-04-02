/*
 * LightUp.c
 *
 * Created: 2/23/2020 11:18:26 AM
 * Circuit: pushbutton from pin 6 (PB1) to ground, 0.1uF capacitor in parallel
 * pin 2 (PB3) is output, resistor 1k to base 2N2222 that controls LED
 * other input pins are not connected
 *
 * Operation: MCU is in sleep mode
 * pushing button wakes it up, turns on LED for 15s, goes back to sleep
 * pushing button again while LED is on, resets 15s timer
 *
 * Current: very roughly - 0.1uA when in sleep, 0.63mA when awake (excluding the LED current)
 */ 

#define LED_PIN					PB3		//output for LED
#define BUTTON_PIN				PB1		//input for button
#define LIGHT_DURATION			65		//arbitrary number for duration, 65 ~ 15s

#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile uint8_t time_counter;

int main (void)
{
	DDRB |= (1 << LED_PIN);										// Set LED as output
	PORTB|= (1 << BUTTON_PIN);									//enable pull-up resistor on button input pin
	PORTB|= (1 << PB0) | (1 << PB2) | (1 << PB4) | (1 << PB5);	//enable pull-up resistors on unused inputs

	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	MCUCR |= 0 << ISC00 | 0 << ISC01;					//enable low level interrupt on INT0 (PB1)
	GIMSK |= (1 << INT0);								//enable external interrupt
	sei();												//enable global interrupts
	
	TCCR0B |= _BV(CS02)|_BV(CS00);						// set prescaler to 1024
	TIMSK0 |= _BV(TOIE0);								// enable Timer Overflow interrupt
	
	while (1) {
		PORTB &= ~(1 << LED_PIN);						//turn the LED off
		sleep_mode();

		PORTB |= (1 << LED_PIN);						//turn the LED on
		while (time_counter < LIGHT_DURATION)
		{
														//empty loop, waste time
		}
		time_counter = 0;								//reset time
	}
}

ISR (INT0_vect)
{
	time_counter = 0;									//wake up and reset time (in case light is on already)
}

ISR (TIM0_OVF_vect)
{
	++time_counter;
}
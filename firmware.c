/*
 * firmware.c
 *
 * Created: 12/4/2016 1:47:58 AM
 *  Author: Tigran
 */ 


#define F_CPU 128000UL
#include <avr/io.h>
#include <util/delay.h>


/*
PIN1 (PB5) BEEPER
PIN2 (PB3) OPERATION ON / OFF SWITCH
PIN3 (PB4) OVERHEAT PROTECTION SWITCH
PIN4 GND
PIN5 (PB0) BLUE STATUS / STAND-BY LED
PIN6 (PB1) RED LED / TRIAC SWITCH
PIN7 (PB2) GREEN LED / RELAY SWITCH
PIN8 VCC
*/


volatile char isON = 0;

volatile int standby = 0;
volatile int flash = 0;
volatile char ledON = 1;


volatile char beepOnce = 0;
volatile char normalStatus = 1;



void beep() {
	_delay_ms(100);
	PORTB |= (1 << PB5);
	_delay_ms(100);
	PORTB &= ~(1 << PB5);
}


void switchON() {
	
	isON = 1;
	
	PORTB |= (1 << PB5);	// beep on
	PORTB &= ~(1 << PB0);	// blue led off
	PORTB |= (1 << PB1);	// triac on
	_delay_ms(150);			// wait
	PORTB |= (1 << PB2);	// relay on
	_delay_ms(200);			// wait
	PORTB &= ~(1 << PB1);	// triac off
	PORTB &= ~(1 << PB5);	// beep off
}


void switchOFF() {
	PORTB |= (1 << PB5);	// beep on
	PORTB |= (1 << PB1);	// triac on
	_delay_ms(150);			// wait
	PORTB &= ~(1 << PB2);	// relay off
	_delay_ms(200);			// wait
	PORTB &= ~(1 << PB1);	// triac off
	PORTB &= ~(1 << PB5);	// beep off
	isON = 0;
	
}



int main(void)
{
	//// INIT ///////////////////////////////
	
	// output pins
	DDRB |= (1 << PB5); // beeper
	DDRB |= (1 << PB0); // status (blue) led
	DDRB |= (1 << PB1); // triac control
	DDRB |= (1 << PB2); // relay control
	
	PORTB &= ~(1 << PB5); // beeper
	PORTB &= ~(1 << PB0); // status (blue) led
	PORTB &= ~(1 << PB1); // triac control
	PORTB &= ~(1 << PB2); // relay control
	
	 
	// input pins
	DDRB &= ~(1 << PB3); // operation switch
	DDRB &= ~(1 << PB4); // overheat switch
	
	PORTB |= (1 << PB3); // operation switch
	PORTB |= (1 << PB4); // overheat switch
	
	////////////////////////////////////////
	
	beep();
	
	
    while(1)
    {
		if (!(PINB & (1 << PB4))) {
			if (isON) {
				switchOFF();
			}
			PORTB |= (1 << PB0);
			if (!beepOnce) {
				beepOnce = 1;
				normalStatus = 0;
				beep();
				beep();
				beep();
			}
		}
		else {
			beepOnce = 0;
			if (normalStatus == 0) {
				normalStatus = 1;
				beep();
			}
			if (!(PINB & (1 << PB3)) && !isON) {
				switchON();
			}
			else if ((PINB & (1 << PB3)) && isON) {
				switchOFF();
			}
		
			if (!isON)
			{
				if (flash == 1000) { ledON = 1;  flash = 0; PORTB &= ~(1 << PB0); }
				if (standby == 15000) { ledON = 0;  standby = 0; PORTB |= (1 << PB0); }
				if (ledON) { standby++; } else { flash++; }
			}	
		}			
	}					 
}

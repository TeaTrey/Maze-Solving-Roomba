/*
 * sonar.c
 *
 *  Created on: Jun 12, 2017
 *      Author: atristan
 */
#include "timer.h"
#include "driverlib/interrupt.h"

#include "lcd.h"
volatile enum {LOW, HIGH, DONE} state;
volatile unsigned rising_time; // start time of the return pulse
volatile unsigned falling_time; // end time of the return pulse
volatile unsigned old; // old tbr_r
int overflow = 0;

int getDistanceSONAR();
unsigned distance;

/* ping sensor related to ISR */
void TIMER3B_Handler(void)
{
	if(TIMER3_RIS_R & TIMER_RIS_CBERIS){
		if(state == LOW){
			rising_time = TIMER3_TBR_R; // Set Rise
			state = HIGH;

		}else if (state == HIGH){
			falling_time = TIMER3_TBR_R;
			state = DONE;
		}
		if(rising_time > falling_time){
					overflow++;
		}
	}
	TIMER3_ICR_R |= TIMER_RIS_CBERIS;
}

int getDistanceSONAR(){
	return (falling_time-rising_time)*(.010625);
	//lcd_printf("%u      %d", distance/10, overflow);
}



void timer_init(){

	state = LOW;
	SYSCTL_RCGCTIMER_R |= 0x8; //Turn on clock to TIMER3


	TIMER3_CTL_R &= 0xFEFF;//disable clock B before changing settings
	TIMER3_CTL_R |= 0x0C00;//sets GPTM timer B event mode to triggr on both edges?
	TIMER3_CFG_R |= 0x4; //Set config to split mode between B.
	TIMER3_TBMR_R |= (0x3 | 0x4 | 0x10); //Setting timer B to capture mode, capture mode to timer, and setting timer to count up.
	TIMER3_TBILR_R = 0xFFFF; // Set load value for the n second clock.
	TIMER3_ICR_R |= (0x400); //Clears TIMER3_B capture interrupt flags
	TIMER3_IMR_R |= (0x400); //Enable TIMER3_B capture interrupts
	NVIC_EN1_R = 0x10; //Enable interrupt for TIMER3_b (interrupt 36)


	TIMER3_CTL_R |= 0x0100;//reenable clock B
}

void gpio_init(){
	SYSCTL_RCGCGPIO_R |= 0b000010;
}
void send_pulse()
{
	TIMER3_CTL_R &= ~TIMER_CTL_TBEN;
	GPIO_PORTB_AFSEL_R &= 0xF7; //Disable alternate
	GPIO_PORTB_PCTL_R |= 0x0000;
	GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output
	GPIO_PORTB_DEN_R |= 0x08;
	GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high

	timer_waitMillis(1);
	state = LOW;

	GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low
	GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input
	GPIO_PORTB_AFSEL_R |= 0x08; //Enable
	GPIO_PORTB_PCTL_R |= 0x7000;

	TIMER3_CTL_R |= TIMER_CTL_TBEN;
}

//int main(void) {
//	gpio_init();
//	timer_init();
//	lcd_init();
//	IntRegister(INT_TIMER3B, TIMER3B_Handler); //Register TIMER3B interrupt handler
//		IntMasterEnable();
//
//	while(1){
//		send_pulse();
//		getDistance();
//		timer_waitMillis(1000);
//	}
//
//}


unsigned ping_read()
{
send_pulse(); // send the starting pulse to PING
// TODO get time of the rising edge of the pulse
// TODO get time of the falling edge of the pulse
// Calculate the width of the pulse; convert to centimeters
}

///* convert time in clock counts to single-trip distance in mm */
//unsigned time2dist(unsigned time)
//{
//…
//}

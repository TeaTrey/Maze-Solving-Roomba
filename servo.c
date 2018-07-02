/*
 * servo.c
 *
 *  Created on: Jun 14, 2017
 *      Author: atristan
 */
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>

#include "button.h"
#include "lcd.h"
#include "timer.h"

int pulse_period = 320000; // pulse period in cycles
int mid_width = 20500;
int degreeSum = 0;
int positive = 1;
char dir = "L";
int button = 0;

uint8_t button_getButton();
void lcd_printf(const char *format, ...);
//int right_width = 14000;
void timer1_init()
{
	//aflsel dir den pctl
	SYSCTL_RCGCGPIO_R |= 0b000010;//gpio clock on
	GPIO_PORTB_AFSEL_R |= 0b100000; //afsel pb5
	GPIO_PORTB_DIR_R |= 0b100000; //pb5 output
	GPIO_PORTB_DEN_R |= 0b100000; //digitalEN pb5
	GPIO_PORTB_PCTL_R |= 0x00700000;

	SYSCTL_RCGCTIMER_R |= 0x00000002; //turn on clk for timer1
	TIMER1_CTL_R &= ~TIMER_CTL_TBEN; //disable timer to config
	TIMER1_CFG_R |= 0x4; //set size of timer to 16
	TIMER1_TBMR_R = 0b1010;//periodic and PWM enable ?page 735 bit9 PWM
	TIMER1_TBPR_R |= (pulse_period >> 16);
	TIMER1_TBILR_R = pulse_period & 0xFFFF; //lower 16 bits of the interval
	//set the upper 8 bits of the interval
	TIMER1_TBMATCHR_R  = (pulse_period - mid_width) & 0xFFFF; // if you want to move servo to the middle
	TIMER1_TBPMR_R  = (pulse_period - mid_width) >> 16; // if you want to move servo to the middle


	//move_servo(63);
	TIMER1_CTL_R |= TIMER_CTL_TBEN;  //enable timer
	//timer_waitMillis(1000);
}

void move_servo(unsigned degree)
{
	//5900 90
	//34500 180

	if(degree < 0 ){
		move_servo(0);
		return;
	}
	if(degree > 180 ){
			move_servo(180);
			return;
		}
//


	unsigned pulse_width = degree*159 + 5900; // pulse width in cycles

	TIMER1_TBMATCHR_R = (pulse_period - pulse_width) & 0xFFFF; // set pulse width
	TIMER1_TBPMR_R = (pulse_period - pulse_width) >> 16;
	timer_waitMillis(50);
}

//void main(){
//
//    button_init();
//	timer1_init();
//	lcd_init();
//	while(1){
//		button = button_getButton();
//		if(positive){
//		    dir = 'L';
//		    if(button == 6){
//		        degreeSum ++;
//		    }else if(button == 5){
//		        degreeSum += 2.5;
//		    }else if(button == 4){
//		        degreeSum += 5;
//		    }else if(button == 3){
//		        positive = 0;
//		    }
//		}else{
//		    dir = 'R';
//		    if(button == 6){
//		        degreeSum --;
//		    }else if(button == 5){
//		        degreeSum -= 2.5;
//		    }else if(button == 4){
//		        degreeSum -= 5;
//		    }else if(button == 3){
//		        positive = 1;
//		    }
//		}
//
//		lcd_printf("Button:%d\nWidth:%d\nDirection:%c", button, degreeSum, dir);
//		move_servo(degreeSum);
//		timer_waitMillis(10);
//	}
//
//
//
//}

/*
 * uart.c
 *
 *  Created on: Jun 5, 2017
 *      Author: atristan
 */
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <string.h>
#include "button.h"
#include "lcd.h"
#include "servo.h"
#include "sonar.h"

char but6[] = "YES";
char but5[] = "NO";
char but4[] = "Blue, no green, Ahhhhh!!!";
char but3[] = "Hello, ";
char but2[] = "it's me. ";
char but1[] = "I was wondering if after all these years you'd like to meet?";
int flag = 0;

//Initialize USART1 to a given baud rate
void uart_init(void) {
	//enable clock to GPIO, R1 = port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	//enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx (See DataSheet)
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
	//enable alternate functions on port b pins 0 and 1
	GPIO_PORTB_AFSEL_R |= 0x03;
	//enable Rx and Tx on port B on pins 0 and 1
	GPIO_PORTB_PCTL_R |= 0x00000011;
	//set pin 0 and 1 to digital
	GPIO_PORTB_DEN_R |= 0x03;
	//set pin 0 to Rx or input
	GPIO_PORTB_DIR_R &= 0b11111110;
	//set pin 1 to Tx or output
	GPIO_PORTB_DIR_R |= 0b00000010;
	//calculate baudrate
	uint16_t iBRD = 8;//use equations
	uint16_t fBRD = 44;//use equations
	//turn off uart1 while we set it up
	UART1_CTL_R &= ~(UART_CTL_UARTEN);
	//set baud rate
	UART1_IBRD_R = iBRD;
	UART1_FBRD_R = fBRD;
	//set frame, 8 data bits, 1 stop bit, no parity, no FIFO
	UART1_LCRH_R = UART_LCRH_WLEN_8 ;
	//use system clock as source
	UART1_CC_R = UART_CC_CS_SYSCLK;
	//re-enable enable RX, TX, and uart1
	UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

//Blocking call that sends 1 char over UART 1
void uart_sendChar(char data)
{
	//wait until there is room to send data
	while(UART1_FR_R & 0x20)
	{
	}
	//send data
	UART1_DR_R = data;
}

//Blocking call to receive over uart1
//returns char with data
char uart_receive(void){
	char data = 0;
	//	int button;
	//	int flag = 0;
	//wait to receive
	while(UART1_FR_R & UART_FR_RXFE)
	{
		//buttonChecker();
	}
	//mask the 4 error bits and grab only 8 data bits
	data = (char)(UART1_DR_R & 0xFF);
	return data;
}

void printStr(char str[]){
	int i = 0;
	for(i=0; i<strlen(str); i++){
		uart_sendChar(str[i]);
	}

}

///Checks buttons to send pre determined strings to the robot LCD screen.
void buttonChecker(void){
	int button = button_getButton();
	if(button == 1 && flag == 0){
		printStr(but6);
		flag = 1;
	}else if(button == 2 && flag == 0){
		printStr(but5);
		flag = 1;
	}else if(button == 3 && flag == 0){
		printStr(but4);
		flag = 1;
	}else if(button == 4 && flag == 0){
		printStr(but3);
		flag = 1;
	}else if(button == 5 && flag == 0){
		printStr(but2);
		flag = 1;
	}else if(button == 6 && flag == 0){
		printStr(but1);
		flag = 1;
	}
	if(button == 0){
		flag = 0 ;
	}
}

//Sends an array of characters to act as a password for the new wifi network
void wifi_sendPass(){
	char str[] = "fuckfuckf";//Password to be sent.
	int i = 0;
	for(i=0; i<9; i++){
		uart_sendChar(str[i]);
	}
	uart_sendChar('\0');//Ending with a null character.
}

// Enables the appropriate pins to enable the wifi connection based on the lab guide.
void wifiStart(){
	/*	Initialize the UART module
	Configure the WiFi command pin (PB2) as output
	Set the command pin to high to enter command mode
	Over UART, send the WIFI_START command (0x00), followed by the password for the network (between 8-32 characters), and a null terminator (0)
	Wait to receive a character from the WiFi bridge. A zero indicates success
	Set the command pin to low to leave command mode
	 */
	uart_init();
	GPIO_PORTB_AFSEL_R &= 0b11111011;
	GPIO_PORTB_DEN_R |= 0b00000100;
	GPIO_PORTB_DIR_R |= 0b00000100; // config pin as output
	GPIO_PORTB_DATA_R |= 0b00000100; //Enter command mode
	uart_sendChar(0); //Send command
	wifi_sendPass();
	char response = uart_receive(); //Wait for response
	lcd_printf("%c", response);
	GPIO_PORTB_DATA_R &= 0b11111011; //Leave command mode
}

void sendInt(int num, int formatSpaces){
	int count = 0;
	int n = num;
	int i = 0;
	int trucount = 0;
	if(num == 0){
		uart_sendChar('0');
		uart_sendChar(' ');
		uart_sendChar(' ');
		return;
	}
	while (n != 0)
	{
		n /= 10;
		count++;
	}
	trucount = count;
	if(count == 0){
		return;
	}
	char charAray[3];
	if(count!=0){
		count--;
		n = num;
		while (n != 0){
			charAray[count] = (n % 10) + '0';
			n /= 10;
			count--;
		}
	}
	for(i=0; i<trucount; i++){
		uart_sendChar(charAray[i]);
	}
	if(formatSpaces){
		if(trucount == 2){
			uart_sendChar(' ');
		}else if(trucount == 1){
			uart_sendChar(' ');
			uart_sendChar(' ');
		}
	}
}

void uart_nextLine(){
	uart_sendChar('\n');
	uart_sendChar('\r');
}
//void wifiStop(){
//	commandPin = 1 //Enter command mode
//			uart_sendChar(WIFI_STOP_COMMAND) //Send command
//	response = uart_waitForChar() //Wait for response
//	commandPin = 0 //Leave command mode
//	if(response != 0) {
//		//An error occurred…
//	}
//}







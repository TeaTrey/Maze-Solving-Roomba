/*
 * uart.h
 *
 *  Created on: Jun 5, 2017
 *      Author: atristan
 */

#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_sendChar(char data);
char uart_receive(void);
void wifiStart(void);
void sendInt(int num, int formatSpaces);
void uart_nextLine(void);
void printStr(char str[]);


#endif /* UART_H_ */

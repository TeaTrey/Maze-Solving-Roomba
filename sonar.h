/*
 * sonar.h
 *
 *  Created on: Jun 12, 2017
 *      Author: atristan
 */

#ifndef SONAR_H_
#define SONAR_H_

void timer_init(void);
void TIMER3B_Handler(void);
void send_pulse(void);
int getDistanceSONAR(void);





#endif /* SONAR_H_ */

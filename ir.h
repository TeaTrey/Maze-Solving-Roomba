/*
 * ir.h
 *
 *  Created on: Jun 26, 2017
 *      Author: atristan
 */

#ifndef IR_H_
#define IR_H_

int getDistanceIR(int num);
int getValue(void);
void adc_init(void);
void storeVals(int newVal);
int getAverage(int values[]);

#endif /* IR_H_ */

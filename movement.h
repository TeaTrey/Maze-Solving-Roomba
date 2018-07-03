/*
 * movement.h
 *
 *  Created on: May 22, 2017
 *      Author: atristan
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

void goStraight(int distanceGo);
void goStraightBack(int distanceGo);
void pivotLeft(int degree);
void pivotRight(int degree);
void stop(void);
void dodgeRight(void);
int goStraightNoFancy(int distanceGo);
void dodgeLeft(void);




#endif /* MOVEMENT_H_ */

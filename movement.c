/*
 * movement.c
 *
 *  Created on: May 22, 2017
 *      Author: atristan
 */

#include "movement.h"
#include "open_interface.h"
#include "uart.h"

int quickSum;
int distanceSum = 0;
int angleSum;
int maxDistance = 2000;
int totalSum = 0;
double angleMult = .51;//Bot 10
double distanceMult = .93;//Bot 10

void pivotLeft(int degree){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	angleSum = 0;
	oi_setWheels(100, -100); // move forward; full speed
	oi_update(sensor_data);
	while (angleSum < degree*angleMult) {
		oi_update(sensor_data);
		angleSum += sensor_data->angle;
	}
	oi_setWheels(0, 0);
	oi_free(sensor_data);

}

void pivotRight(int degree){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);

	angleSum = 0;
	oi_setWheels(-100, 100); // move forward; full speed
	oi_update(sensor_data);
	while (angleSum < degree*angleMult) {
		oi_update(sensor_data);
		angleSum -= sensor_data->angle;
	}
	oi_setWheels(0, 0);
	oi_free(sensor_data);
}

void dodgeRight(){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	goStraightBack(150);
	pivotRight(90);
	goStraight(250);
	pivotLeft(90);
	goStraight(150);
	oi_free(sensor_data);
}

void dodgeLeft(){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	goStraightBack(150);
	pivotLeft(90);
	goStraight(250);
	pivotRight(90);
	goStraight(150);
	oi_free(sensor_data);
}

void goStraight(int distanceGo){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	quickSum = 0;
	oi_setWheels(250, 250); // move forward; full speed
	while (quickSum < distanceGo*distanceMult) {
		oi_update(sensor_data);
		if (sensor_data->bumpLeft) {
			oi_update(sensor_data);
			angleSum = 0;
			totalSum=quickSum;
			dodgeRight();
			quickSum = totalSum;
			oi_setWheels(250, 250);
			angleSum = 0;
		}
		if (sensor_data->bumpRight) {
			oi_update(sensor_data);
			angleSum = 0;
			totalSum=quickSum;
			dodgeLeft();
			quickSum = totalSum;
			oi_setWheels(250, 250);
			angleSum = 0;
		}
		if (sensor_data->bumpRight && sensor_data->bumpLeft) {
			oi_update(sensor_data);
			angleSum = 0;
			totalSum=quickSum;
			dodgeLeft();
			quickSum = totalSum;
			oi_setWheels(250, 250);
			angleSum = 0;
		}
		quickSum += sensor_data->distance;
		angleSum = 0;
	}
	oi_setWheels(0, 0);
	oi_free(sensor_data);

}



int goStraightNoFancy(int distanceGo){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	quickSum = 0;
	if(sensor_data->bumpLeft || sensor_data->bumpRight|| sensor_data->cliffFrontLeft ||
			sensor_data->cliffFrontRight || sensor_data->cliffLeft || sensor_data->cliffRight){//Dont move if something is already wrong.
		return quickSum;
	}else{
		oi_setWheels(250, 250);



		while (quickSum < distanceGo*distanceMult) {

			oi_update(sensor_data);
			if (sensor_data->bumpLeft) {
				oi_setWheels(0, 0);
				printStr("We bumped on left. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->bumpRight) {
				oi_setWheels(0, 0);
				printStr("We bumped on right. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->bumpRight && sensor_data->bumpLeft) {
				oi_setWheels(0, 0);
				printStr("We bumped on center. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->cliffFrontLeft) {//actual cliff left
				oi_setWheels(0, 0);
				printStr("Cliff on front left. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->cliffFrontRight) {//actual cliff right
				oi_setWheels(0, 0);
				printStr("Cliff on front right. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->cliffFrontRight && sensor_data->cliffFrontLeft) {//actual cliff
				oi_setWheels(0, 0);
				printStr("Cliff directly ahead. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->cliffRight) {//actual cliff
				oi_setWheels(0, 0);
				printStr("Cliff on right. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if (sensor_data->cliffLeft) {//actual cliff
				oi_setWheels(0, 0);
				printStr("Cliff on left. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if(sensor_data->lightBumpFrontRightSignal > 200 || sensor_data->lightBumpFrontLeftSignal > 200 || sensor_data->lightBumpLeftSignal > 200
					|| sensor_data->lightBumpRightSignal > 200 ){//Stop if light sensors see shit.
				oi_setWheels(0, 0);
				printStr("Bump light trigger. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			if(sensor_data->cliffLeftSignal > 2600 || sensor_data->cliffFrontLeftSignal > 2600 || sensor_data->cliffFrontRightSignal > 2600
					|| sensor_data->cliffRightSignal > 2600 ){//Stop if light sensors see shit.
				oi_setWheels(0, 0);
				printStr("Boundary trigger. ");
				oi_update(sensor_data);
				oi_free(sensor_data);
				return quickSum;
			}
			quickSum += sensor_data->distance;
		}
		oi_setWheels(0, 0);
		oi_free(sensor_data);
		return quickSum;
	}
}

void goStraightBack(int distanceGo){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	quickSum = 0;
	oi_setWheels(-250, -250); // move backwards; half speed
	//oi_update(sensor_data);
	while (quickSum < distanceGo*distanceMult) {
		lcd_printf("%d, %d", distanceGo, quickSum);
		oi_update(sensor_data);
		if(sensor_data->distance > 0){
			quickSum += sensor_data->distance;
		}else{
		quickSum -= sensor_data->distance;
		}
	}
	oi_setWheels(0, 0);
	oi_free(sensor_data);
}

void stop(){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	oi_setWheels(0, 0); // stop
	oi_free(sensor_data);
}

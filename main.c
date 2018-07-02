/*
 * main.c
 */

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "button.h"
#include "lcd.h"
#include "servo.h"
#include "ir.h"
#include "uart.h"
#include "sonar.h"
#include "Math.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
#include "open_interface.h"

#include "movement.h"
#include "music.h"

int sonarDistance = 0;
int irDistance = 0;
int degreeSums = 0;
int lastIR = 0;
int lastSONAR = 0;
int IRDiff = 0;
int SONARDiff = 0;
int firstObjects[10] = {};
int lastObjects[10] = {};
int objects[10] = {};
int objectsDistance[10] = {};
int objectsWidth[10] = {};
int k = 0;
int avgDist = 0;
int firstFlag = 0;
int i = 0;
int closestDistance = 100000;
int closestObject = 0;

int objectCount = 0;
int smallestWidth = 100000;
int smallestObject = 0;
int distanceLastDriven = 0;
int bigFlag = 0;
int smallFlag = 0;
int distanceGone = 0;
char received;
char new;
int turn;

void saveValues(){
	send_pulse(); //Send pulse for sonar
	int sonar = getDistanceSONAR();
	sonarDistance = sonar/10;//Calculate sonar distance
	if(sonarDistance > 1000000){ //Throw out shitty numbers
		sonarDistance = lastSONAR;
	}
	irDistance = 69045*pow(getValue(), -1.103);//Calculate IR distance
}

void initAll(){
	//All inits
	timer1_init();
	timer_init();
	button_init();
	lcd_init();
	uart_init();
	adc_init();
	IntRegister(INT_TIMER3B, TIMER3B_Handler); //Register TIMER3B interrupt handler
	IntMasterEnable();
	//	oi_t *sensor_data = oi_alloc();
	//	oi_init(sensor_data);
	//load_songs();

}

void scan(){
	degreeSums = 0;
	move_servo(degreeSums);
	timer_waitMillis(600);//time it needs to reset position to zero degrees. DONT CHANGE
	while(1){
		move_servo(degreeSums);
		saveValues();
		avgDist = (sonarDistance + irDistance) / 2; //Calculate average of the two distances
		if(irDistance < 60 && k <=10 && firstFlag == 0){//If withing range, mark the starting angle
			if(smallFlag == 0){
				smallFlag = 1;
			}else if(smallFlag == 1){
				firstObjects[k] = degreeSums;//Mark angle
				objectsDistance[k] = avgDist;//Save distance
				firstFlag = 1;//Set flag
				bigFlag = 0;
				smallFlag = 0;
			}
		}
		if(irDistance > 60 && k <=10 && firstFlag == 1){//If out of range, mark end angle
			if(bigFlag == 0){
				bigFlag = 1;
			}else if(bigFlag == 1){
				lastObjects[k] = degreeSums - 2;//Mark angle
				firstFlag = 0;//Clear Flag
				k++;//Increment objects
				objectCount++;//Increment objects
				bigFlag = 0;
			}

		}
		timer_waitMillis(20);//Slow down to read
		lcd_printf("sonar:%d, ir:%d", sonarDistance, irDistance);//Print distances to LCD
		sendInt(degreeSums,1);//Send to PuTTY
		uart_sendChar(' ');
		sendInt(irDistance,1);//Send to PuTTY
		uart_sendChar(' ');
		sendInt(sonarDistance,1);//Send to PuTTY
		uart_sendChar(' ');
		sendInt(avgDist,1);//Send to PuTTY
		uart_nextLine();
		if(degreeSums >= 180){//The scan is over
			for(i = 0; i<3; i++){//For loop to do all the array stuff
				objects[i] = ((firstObjects[i] + lastObjects[i]) / 2);// - 10;//Average angle location of each object
				objectsWidth[i] = (lastObjects[i] - firstObjects[i]); //Angle width of each object
				if(objectsDistance[i] != 0 &&  objectsDistance[i]< closestDistance){//Decide which object is closest.
					closestDistance = objectsDistance[i];
					closestObject = i;

				}
				if(objectsWidth[i] != 0 && objectsWidth[i] < smallestWidth){//Decide which object is smallest
					smallestWidth = objectsWidth[i];
					smallestObject = i;

				}
			}

			//			move_servo(objects[smallestObject]);//Point to the smallest.
			//			uart_nextLine();
			//			sendInt(smallestWidth);//Send to PuTTY
			//			uart_sendChar(' ');
			//			sendInt(objects[smallestObject]);//Send to PuTTY
			//			uart_sendChar(' ');
			//			sendInt(objectsDistance[smallestObject]);//Send to PuTTY
			//			uart_sendChar(' ');
			//			sendInt(objectCount);//Send to PuTTY
			//			uart_sendChar(' ');
			//			uart_nextLine();
			//			uart_nextLine();
			//        	//Calculate LINAER WITDHT!
			//        	double sinNum = sin(smallestWidth/2.0);
			//int dist = objectsDistance[smallestObject];
			//        	double value = angleWidth * sinNum * 3;
			//        	int send = 0;
			//        	if(value<0){
			//        	send = value * -1;
			//        	}else{
			//        	send = value;
			//        	}
			//        	sendInt(send/10);
			//int send = fabs(2*(dist*tan((smallestWidth*3.1415926535897932384)/360)));
			//sendInt(send - 9);
			break;
		}else{
			degreeSums += 2;
		}

	}
}

void pointAtSkinny(){
	if(objects[smallestObject] > 90 && objects[smallestObject] < 135){
		pivotLeft(135-objects[smallestObject]);
	}else if(objects[smallestObject] < 90 && objects[smallestObject] > 45){
		pivotRight(objects[smallestObject] - 45);
	}
}

void getSensorData(){
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	uart_nextLine();
	uart_nextLine();
	printStr("!!CURRENT SENSOR DATA!!");
	uart_nextLine();
	printStr("-Bump Sensors-");
	uart_nextLine();
	printStr("Left: ");
	sendInt(sensor_data->bumpLeft,0);
	uart_nextLine();
	printStr("Right: ");
	sendInt(sensor_data->bumpRight,0);
	uart_nextLine();
	printStr("-Cliff Sensors-");
	uart_nextLine();
	printStr("Left: ");
	sendInt(sensor_data->cliffLeft,0);
	uart_nextLine();
	printStr("Front Left: ");
	sendInt(sensor_data->cliffFrontLeft,0);
	uart_nextLine();
	printStr("Right: ");
	sendInt(sensor_data->cliffRight,0);
	uart_nextLine();
	printStr("Front Right: ");
	sendInt(sensor_data->cliffFrontRight,0);
	uart_nextLine();
	printStr("-Cliff Signal Sensors-");
	uart_nextLine();
	printStr("Left: ");
	sendInt(sensor_data->cliffLeftSignal,0);
	uart_nextLine();
	printStr("Front Left: ");
	sendInt(sensor_data->cliffFrontLeftSignal,0);
	uart_nextLine();
	printStr("Right: ");
	sendInt(sensor_data->cliffRightSignal,0);
	uart_nextLine();
	printStr("Front Right: ");
	sendInt(sensor_data->cliffFrontRightSignal,0);
	uart_nextLine();
	printStr("-Light Bump Sensors-");
	uart_nextLine();
	printStr("Left: ");
	sendInt(sensor_data->lightBumpLeftSignal,0);
	uart_nextLine();
	printStr("Front Left: ");
	sendInt(sensor_data->lightBumpFrontLeftSignal,0);
	uart_nextLine();
	printStr("Center Left: ");
	sendInt(sensor_data->lightBumpCenterLeftSignal,0);
	uart_nextLine();
	printStr("Right: ");
	sendInt(sensor_data->lightBumpRightSignal,0);
	uart_nextLine();
	printStr("Front Right: ");
	sendInt(sensor_data->lightBumpFrontRightSignal,0);
	uart_nextLine();
	printStr("Center Right: ");
	sendInt(sensor_data->lightBumpCenterRightSignal,0);
	uart_nextLine();
	uart_nextLine();
	uart_nextLine();
	oi_free(sensor_data);
}

void controls(char received){
	switch(received) {
	case 'w' :
		printStr("Go straight. 1-5cm, 2-20cm, 3-100cm");
		uart_nextLine();
		new = uart_receive();
		turn = 0;
		if(new == '1') turn = 50;
		else if(new == '2') turn = 200;
		else if(new == '3') turn = 1000;
		else{
			printStr("Invalid Code!");
			uart_nextLine();
			break;
		}
		distanceGone = goStraightNoFancy(turn);
		printStr("Moved forward ");
		sendInt(distanceGone, 0);
		printStr(".");
		uart_nextLine();
		break;
	case 'c' :
		scan();
		printStr("Scan complete.\n");
		uart_nextLine();
		break;
	case 'q' :
		printStr("Pivot left. 1-5, 2-10, 3-20, 4-90");
		uart_nextLine();
		new = uart_receive();
		turn = 0;
		if(new == '1') turn = 5;
		else if(new == '2') turn = 10;
		else if(new == '3') turn = 20;
		else if(new == '4') turn = 90;
		else{
			printStr("Invalid Code!");
			uart_nextLine();
			break;
		}
		pivotLeft(turn);
		printStr("Pivoted left ");
		sendInt(turn, 0);
		printStr(" degrees.");
		uart_nextLine();
		break;
	case 'e' :
		printStr("Pivot right. 1-5, 2-10, 3-20, 4-90");
		uart_nextLine();
		new = uart_receive();
		turn = 0;
		if(new == '1') turn = 5;
		else if(new == '2') turn = 10;
		else if(new == '3') turn = 20;
		else if(new == '4') turn = 90;
		else{
			printStr("Invalid Code!");
			uart_nextLine();
			break;
		}
		pivotRight(turn);
		printStr("Pivoted right ");
		sendInt(turn, 0);
		printStr(" degrees.");
		uart_nextLine();
		break;
	case 's' :
		goStraightBack(100);
		printStr("Moved backwards 100.");
		uart_nextLine();
		break;
	case 'g' :
		getSensorData();
		break;
	case 'z' :
		playRickRoll();
		printStr("Played Rick Roll.");
		uart_nextLine();
		break;
	default :
		printf("Invalid command.\n" );
	}
}


int main(void) {
	int werfer = 0;
	if(werfer == 1){
		wifiStart();
	}else if(werfer == 0){
		initAll();

		//		oi_t *sensor_data = oi_alloc();
		//		oi_init(sensor_data);


		//While loop
		//oi_setWheels(100, 100);

		while(1){
			/* local variable definition */
			received = uart_receive();
			timer_waitMillis(500);
			lcd_printf("%c", received);
			controls(received);
			//			oi_setWheels(100, 100);
			//			oi_update(sensor_data);
			//			distanceLastDriven += sensor_data->distance;
			//			//lcd_printf("fr: %d fl: %d\nl: %d r: %d\nd: %d", sensor_data->lightBumpFrontRightSignal, sensor_data->lightBumpFrontLeftSignal
			//					//, sensor_data->lightBumpLeftSignal, sensor_data->lightBumpRightSignal, distanceLastDriven);
			//			if(sensor_data->lightBumpFrontRightSignal > 200 || sensor_data->lightBumpFrontLeftSignal > 200 || sensor_data->lightBumpLeftSignal > 200
			//					|| sensor_data->lightBumpRightSignal > 200 ){
			//
			//				oi_setWheels(0, 0);
			//
			//				if(distanceLastDriven > 150){//if we just moved 35
			//					oi_setWheels(0, 0);
			//					goStraightBack(150);//35mm to scan
			//					scan();
			//					lcd_printf("objects: %d smallest: %d", objectCount, objects[smallestObject]);
			//					break;
			////					//pointAtSkinny();
			//				}else{
			//					oi_setWheels(0, 0);
			//					goStraightBack(distanceLastDriven);
			//					scan();
			//					break;
			//					//pointAtSkinny();
			//				}
			////
			////
			////			}
			////
			//		}
		}
	}
}











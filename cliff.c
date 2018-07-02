/*
 * cliff.c
 *
 *  Created on: Jun 26, 2017
 *      Author: atristan
 */

#include "lcd.h"
#include "open_interface.h"
#include "movement.h"

int white = 2700;

//void main() {
//	lcd_init();
////
//	oi_t *sensor_data = oi_alloc();
//	oi_init(sensor_data);
//	lcd_printf("hello");
////
//////	uint32_t cliffLeft : 1;
//////		uint32_t cliffFrontLeft : 1;
//////		uint32_t cliffFrontRight : 1;
//////		uint32_t cliffRight : 1;
////
////
////lcd_init();
////
////		    oi_setWheels(100, 100); // move forward; full speed
////		    while (1) {
////		        oi_update(sensor_data);
//////		        if (sensor_data->cliffFrontLeft || sensor_data->cliffFrontRight) {//actual cliff
//////		        	oi_update(sensor_data);
//////		        	oi_setWheels(-100, -100);
//////
//////		        }
////		        if (sensor_data->lightBumperRight  >  || sensor_data->cliffFrontRightSignal > 2700) {//white borreder
////		        		        	oi_update(sensor_data);
////		        		        	oi_setWheels(-100, -100);
////
////		        		        }
//	oi_setWheels(100, 100);
//	while(1){
//		oi_update(sensor_data);
//	    // lcd_printf("%d", sensor_data->lightBumpFrontRightSignal);
//		lcd_printf("fr: %d fl: %d l: %d r: %d", sensor_data->lightBumpFrontRightSignal, sensor_data->lightBumpFrontLeftSignal
//			    			   , sensor_data->lightBumpLeftSignal, sensor_data->lightBumpRightSignal);
//	       if(sensor_data->lightBumpFrontRightSignal > 200 || sensor_data->lightBumpFrontLeftSignal > 200 || sensor_data->lightBumpLeftSignal > 200
//	    		   || sensor_data->lightBumpRightSignal > 200 ){
//
//	    	   oi_setWheels(0, 0);
//	       }
//	}
//
////
////
////
////		    }
////		    oi_setWheels(0, 0); // stop
////		    //oi_free(sensor_data);
////
////
////
////	 oi_free(sensor_data);
//}




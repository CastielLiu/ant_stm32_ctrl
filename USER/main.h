#ifndef MAIN_H_
#define MAIN_H_

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "wdg.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "exti.h"
#include "can.h"
#include "beep.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "24l01.h"

typedef struct PkgID1
{
	uint8_t header[2];
	uint8_t pkgLen_H;
	uint8_t pkgLen_L;
	uint8_t pkgID;
	uint8_t driverlessMode : 1;
	uint8_t emergencyBrake : 1;
	uint8_t reversed1      : 6;
	uint8_t reversed2;
	uint8_t checkSum;
	
} pkg_id1_t;

#endif 

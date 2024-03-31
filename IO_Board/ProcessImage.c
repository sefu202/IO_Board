/*
 * ProcessImage.c
 *
 * Created: 31.03.2024 23:59:20
 *  Author: josef
 */ 

#include "ProcessImage.h"
#include "DI.h"
#include "DO.h"
#include "Step1.h"
#include "PWM.h"

static processImage_t ProcessImage;


void processImage_rxFrame(uint8_t *pFrame){
	ProcessImage.rx.DO				= pFrame[0];
	ProcessImage.rx.motor[0].flags	= pFrame[1];
	ProcessImage.rx.motor[0].speed	= pFrame[2];
	ProcessImage.rx.motor[1].flags	= pFrame[3];
	ProcessImage.rx.motor[1].speed	= pFrame[4];
	ProcessImage.rx.motor[2].flags	= pFrame[5];
	ProcessImage.rx.motor[2].speed	= pFrame[6];
	ProcessImage.rx.motor[3].flags	= pFrame[7];
	ProcessImage.rx.motor[3].speed	= pFrame[8];
	
	processImage_writeOutputs();
}

void processImage_writeOutputs(){
	// Write DO
	do_set(ProcessImage.rx.DO);
	
	// Write DC Motors
	PWM_setOutput1(ProcessImage.rx.motor[0].speed);
	PWM_setOutput2(ProcessImage.rx.motor[1].speed);
	
	// Write Stepper Motors
	// Enable, Direction, Speed
	step1(ProcessImage.rx.motor[2].flags & (1 << 0), ProcessImage.rx.motor[2].flags & (1 << 1), ProcessImage.rx.motor[2].speed);
}

void processImage_readInputs(){
	ProcessImage.tx.DI = di_get();
}
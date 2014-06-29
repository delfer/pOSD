#include <stdio.h>
#include "config.h"
#include "stm32f10x_adc.h"

uint16_t data[9500];
uint16_t data_i=0;

uint16_t vSyncLvl=800; //Sync pulse level for analog watchdog
uint8_t vSyncConfigured=0; //Sync configured at current cSyncLvl flag
uint16_t vSyncLineOne=0; //Line time for compare
uint16_t vSyncLineLen=0; //Line time after comparing
uint16_t vSyncSignal=0; //0 - undefined, 1 - PAL, 2 - NTSC
uint16_t vSyncLine=0; //Current line number
uint16_t vSyncLineM=0; //Max lines

uint16_t vSyncErrors=0; //Current count of sync errors

uint8_t vSyncSeq1 = 0;
uint8_t vSyncSeq2 = 0;
uint8_t vSyncSeq3 = 0;

int main (void)
{
	uint32_t i=0;
	
	uint16_t vSyncLvlMin = 0;
	uint16_t vSyncLvlMax = 0;
	
	SetSysClockTo72();
	USARTConfig();
	ADC1config();
	TIM1config();
	printf ("\r\n");
	
	//sync Detector
	for (vSyncLvl=0;vSyncLvl<1000;vSyncLvl++)
	{
		ADC_ITConfig(ADC1, ADC_IT_AWD, DISABLE); //stop AWD
		//reset
		vSyncLineOne=0;
		vSyncLineLen=0;
		vSyncLineM=0;
		vSyncSignal=0;
		vSyncErrors=0;
		vSyncSeq1 = 0;
		vSyncSeq2 = 0;
		vSyncSeq3 = 0;
		//reset AWD
		ADC_AnalogWatchdogThresholdsConfig(ADC1, 0xFFF, 0xFFE);
		ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE); //start AWD
		
		for (i=0; i < 350000; i++) { //wait more,than frame (some errors on 300 000)
			//if (TIM_GetCounter(TIM2) > 5000 || vSyncSignal != 0)
			// || vSyncErrors > 50
			if (TIM_GetCounter(TIM2) > 5000 || vSyncErrors > 0 || vSyncSignal != 0)
				break;
		}
		
		//Check detected signal
		if (vSyncSignal == 1 || vSyncSignal == 2)
			printf ("vSyncLvl (%d), vSyncSignal (PAL), vSyncLineM (%d)\r\n", vSyncLvl, vSyncLineM);
		else if (vSyncSignal == 4 || vSyncSignal == 5)
			printf ("vSyncLvl (%d), vSyncSignal (NTSC), vSyncLineM (%d)\r\n", vSyncLvl, vSyncLineM);
		else
			printf ("vSyncLvl (%d)\r\n", vSyncLvl);
		
		//Set Min Max
		if (vSyncLvlMin == 0 && vSyncSignal != 0)
			vSyncLvlMin = vSyncLvl;
		if (vSyncSignal != 0)
			vSyncLvlMax = vSyncLvl;
		
	}
	
	//Configuring vSync
	if (vSyncLvlMin != 0 && vSyncLvlMax != 0)
	{
		vSyncLvl = (vSyncLvlMin+vSyncLvlMax)/2;
		
		ADC_ITConfig(ADC1, ADC_IT_AWD, DISABLE); //stop AWD
		//reset
		vSyncLineOne=0;
		vSyncLineLen=0;
		vSyncLineM=0;
		vSyncSignal=0;
		vSyncErrors=0;
		vSyncSeq1 = 0;
		vSyncSeq2 = 0;
		vSyncSeq3 = 0;
		//reset AWD
		ADC_AnalogWatchdogThresholdsConfig(ADC1, 0xFFF, 0xFFE);
		ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE); //start AWD
		
		for (i=0; i < 500000; i++) { //wait more,than frame (some errors on 300 000)
			//if (TIM_GetCounter(TIM2) > 5000 || vSyncSignal != 0)
			// || vSyncErrors > 50
			//if (TIM_GetCounter(TIM2) > 5000 || vSyncErrors > 0)
			//	break;
		}
	}
	
	printf ("\r\n===========================\r\n");
	printf ("End of search\r\n");
	printf ("Lines: %d\r\n", vSyncLineM);
	printf ("ADC Sync Lvl: %d\r\n", vSyncLvl);
	for (;;) {
		printf ("Signal type: ");
		if (vSyncSignal == 1 || vSyncSignal == 2)
				printf ("PAL (%d)\r\n", vSyncSignal);
			else if (vSyncSignal == 4 || vSyncSignal == 5)
				printf ("NTSC (%d)\r\n", vSyncSignal);
			else
				printf ("N/A\r\n");
		
		printf ("Current line: %d/%d\r\n", vSyncLine, vSyncLineM);
		vSyncLineM=0;
		for (i=0; i < 500000; i++) { };//wait more,than frame (some errors on 300 000)
	}
	
	
	//FOR DEBUG PURPOSE
	/*
	printf ("\r\nStarted!\r\nvSyncLvl %d\r\n", vSyncLvl);
	
	while (data_i < 9499)
	{
	}
	ADC_ITConfig(ADC1, ADC_IT_AWD, DISABLE);
	for (i=0; i < 9499; i++)
	{
		printf ("%d,", data[i]);
	}
	printf ("\r\n End\r\n");
	for (;;)	{
	}
	*/
}


//web: http://martin.hinner.info/vga/pal.html
//Video timings
//600TVL (visible, 610 total) at 72MHz timer
//string = 2140
//long = 1062 (long pulse = short sync)
//short = 170 (short pulse = long sync)
//PAL 1st 6x long + 5x short + 5x long
//PAL 2nd 5x long + 5x short + 4x long
//PAL prg 6x long + 5x short + 5x long
//NTSC 1st 6x long + 6x short + 6x long
//NTSC 2st 7x long + 6x short + 5x long

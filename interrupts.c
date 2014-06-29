#include "stm32f10x.h"

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles ADC1 and ADC2 global interrupts requests.
  * @param  None
  * @retval None
  */

extern uint16_t data[9500];
extern uint16_t data_i;

//Video sync interrupt
void ADC1_2_IRQHandler(void)
{
	uint16_t timerVal; 
	
	static uint8_t state; // 0 while line, 1 while sync
	static uint8_t vSyncLineDif = 20; //lines between lines for compare
	
	extern uint8_t vSyncSeq1; // Sequense of 5-7 long pulses
	extern uint8_t vSyncSeq2; // Sequense of 5-6 short pulses
	extern uint8_t vSyncSeq3; // Sequense of 4-6 long pulses
	extern uint16_t vSyncLine;
	extern uint16_t vSyncLineM; 
	
	extern uint16_t vSyncErrors;
	
	extern uint16_t vSyncLvl; 
	extern uint8_t vSyncConfigured;
	extern uint16_t vSyncLineOne;
	extern uint16_t vSyncLineLen;
	extern uint16_t vSyncSignal;

	
	if (state == 0)
	{
		//data [data_i++] = TIM_GetCounter(TIM2); //FOR DEBUG PURPOSE
		
		timerVal = TIM_GetCounter(TIM2);
		
		//{BLOCK 1: detecting line len}
		if (vSyncLineLen == 0)
		{
			//Setting line one lenght
			if (vSyncLineOne == 0)
				vSyncLineOne = timerVal;
			//Setting line two lenght
			else if (vSyncLineLen == 0 && vSyncLineDif == 0)
			{
				//fast comparing lines
				if ((timerVal >> 8) != (vSyncLineOne >> 8))
				{
					//if not equal, reseting
					vSyncLineOne = 0;
					vSyncLineLen = 0;
					vSyncLineDif = 20; //More than sync pulses count
				}
				else
					vSyncLineLen = timerVal;
			}
			//Wating for line two (after vSyncLineDif lines)
			else if (vSyncLineDif!=0)
				vSyncLineDif--;
		}
		//{BLOCK 1: detecting frame Sync}
		else if (timerVal < ((vSyncLineLen >> 1) + (vSyncLineLen >> 2))) //less than 3/4 of line len
		{
			if (vSyncLine > vSyncLineM)
				vSyncLineM = vSyncLine;
			vSyncLine = 0;
			//Frame Sync pulses
			if (timerVal <= (vSyncLineLen >> 2)) //less than 1/4 of line len
			{
				//short pulse = 2nd sequense
				if (vSyncSeq1 >= 5 && vSyncSeq2 < 7 && vSyncSeq3 == 0)
					vSyncSeq2++;
				else
				{
					//wrong timings
					vSyncSeq1 = 0;
					vSyncSeq2 = 0;
					vSyncSeq3 = 0;
					vSyncErrors++;
				}
			}
			else
			{
				//long pulse = 1st or 3rd sequense
				if (vSyncSeq2 == 0)
				{
					//1st sequense
					if (vSyncSeq1 < 8 && vSyncSeq2 == 0 && vSyncSeq3 == 0)
						vSyncSeq1++;
					else
					{
						//wrong timings
						vSyncSeq1 = 0;
						vSyncSeq2 = 0;
						vSyncSeq3 = 0;
						vSyncErrors++;
					}
				}
				else
				{
					//3rd sequense
					if (vSyncSeq1 >= 5 && vSyncSeq2 >= 5 && vSyncSeq3 < 7)
						vSyncSeq3++;
					else
					{
						//wrong timings
						vSyncSeq1 = 0;
						vSyncSeq2 = 0;
						vSyncSeq3 = 0;
						vSyncErrors++;
					}
				}
			}
		}
		else
		{
			if (vSyncLine == 0)
			{
				//Normal string
				if (vSyncSeq1 == 6 && vSyncSeq2 == 5 && vSyncSeq3 == 5)
					vSyncSignal = 1;
				else if (vSyncSeq1 == 5 && vSyncSeq2 == 5 && vSyncSeq3 == 4)
					vSyncSignal = 2;
				else if (vSyncSeq1 == 6 && vSyncSeq2 == 6 && vSyncSeq3 == 6)
					vSyncSignal = 4;
				else if (vSyncSeq1 == 7 && vSyncSeq2 == 6 && vSyncSeq3 == 5)
					vSyncSignal = 5;
				else
					vSyncSignal = 0;
				
				vSyncSeq1 = 0;
				vSyncSeq2 = 0;
				vSyncSeq3 = 0;
			}
			vSyncLine++;
		}
	}

	TIM_SetCounter(TIM2, 0);
	
	if (state == 1)
	{
		ADC_AnalogWatchdogThresholdsConfig(ADC1, 0xFFF, vSyncLvl);
		state = 0;
	}
	else
	{
		ADC_AnalogWatchdogThresholdsConfig(ADC1, vSyncLvl, 0);
		state = 1;
	}
	
  /* Clear ADC1 AWD pending interrupt bit */
  ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
}


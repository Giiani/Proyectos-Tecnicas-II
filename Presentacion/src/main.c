/*
******************************************************************************
******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32_ub_lcd_4x20.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "stdio.h"
/* Private macro */
/* Private variables */
uint16_t ADC_ConvertedValue = 0, TempSensor_ConvertedValue = 0, Vref_ConvertedValue = 0, Vbat_ConvertedValue = 0;
char texto[16];
int n=0;
/* Private function prototypes */
void EXTI11_Configure (void);
void GPIO_Configure(void);
void ADC_Configure (void);
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	float vTempSensor = 0, Vbat = 0;

	SystemInit();
	GPIO_Configure ();
	ADC_Configure ();
	EXTI11_Configure();
	UB_LCD_4x20_Init();
	char disparo[16];

  while (1)
  {
	  if (ADC_ConvertedValue <= 30)
	  		{
	  			GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	  		}
	  		else if (ADC_ConvertedValue > 30 && ADC_ConvertedValue < 1024)
	  		{
	  			GPIO_SetBits (GPIOD, GPIO_Pin_13);
	  			GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
	  		}
	  		else if (ADC_ConvertedValue >= 1024 && ADC_ConvertedValue < 2048)
	  		{
	  			GPIO_SetBits (GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
	  			GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_15);
	  		}
	  		else if (ADC_ConvertedValue >= 2048 && ADC_ConvertedValue < 3072)
	  		{
	  			GPIO_SetBits (GPIOD, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	  			GPIO_ResetBits (GPIOD, GPIO_Pin_12);
	  		}
	  		else if (ADC_ConvertedValue >= 3072 && ADC_ConvertedValue < 4096)
	  		{
	  			GPIO_SetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	  		}

	  		vTempSensor = (float) TempSensor_ConvertedValue;	// vTempSensor (mV) = 3300 * (TempSensor_ConvertedValue / 4095)
	  		vTempSensor *= 3300;
	  		vTempSensor /=4095;
	  		vTempSensor /=(float)1000;
	  		vTempSensor -=(float)0.76;
	  		vTempSensor +=(float)25;

	  		Vbat = (float) Vbat_ConvertedValue * 2;					// Internal bridge divider by 2
	  		Vbat /= 4095;											// Vbat (mV) = 3300 * ((Vbat_ConvertedValue * 2) / 4095)
	  		Vbat *= 3300;

	  		sprintf(texto,"Tem:%.2f Bat:%.0f", vTempSensor, Vbat);
	  		UB_LCD_4x20_String(0,0,texto);
	  		sprintf(disparo,"Disparo Nro: %d",n);
	  		UB_LCD_4x20_String(0,1,disparo);
  }
}

void GPIO_Configure (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (GPIOA, &GPIO_InitStructure);

	/* Leds */
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE);

								/* LED4 Green | LED3 Orange |  LED5 Red   | LED6  Blue */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init (GPIOD, &GPIO_InitStructure);
}

void ADC_Configure (void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_Ext_IT11;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 3;
	ADC_Init (ADC1, &ADC_InitStructure);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit (&ADC_CommonInitStructure);

	ADC_TempSensorVrefintCmd (ENABLE);
	ADC_VBATCmd (ENABLE);

	ADC_RegularChannelConfig (ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig (ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig (ADC1, ADC_Channel_Vbat, 3, ADC_SampleTime_3Cycles);

	ADC_EOCOnEachRegularChannelCmd (ADC1, ENABLE);

	ADC_ITConfig (ADC1, ADC_IT_OVR, ENABLE);
	ADC_ITConfig (ADC1, ADC_IT_EOC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_Init (&NVIC_InitStructure);

	ADC_Cmd (ADC1, ENABLE);
}

void EXTI11_Configure (void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig (EXTI_PortSourceGPIOD, EXTI_PinSource11);

	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init (&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init (&NVIC_InitStructure);
}

void ADC_IRQHandler (void)
{
	if (ADC_GetITStatus (ADC1, ADC_IT_EOC) != RESET)
	{
		while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)!=0)
		{

		}
		n++;
		ADC_ConvertedValue = ADC_GetConversionValue (ADC1);
		TempSensor_ConvertedValue = ADC_GetConversionValue (ADC1);
		Vbat_ConvertedValue = ADC_GetConversionValue (ADC1);
		ADC_ClearITPendingBit (ADC1, ADC_IT_EOC);
	}
	else if (ADC_GetITStatus (ADC1, ADC_IT_OVR) != RESET)
	{
		ADC_ClearITPendingBit (ADC1, ADC_IT_OVR);
	}
}

void EXTI15_10_IRQHandler (void)
{
	if (EXTI_GetITStatus (EXTI_Line11) != RESET)
	{
		EXTI_ClearITPendingBit (EXTI_Line11);
	}
}

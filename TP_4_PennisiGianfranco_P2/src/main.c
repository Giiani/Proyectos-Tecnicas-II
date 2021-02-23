/*
******************************************************************************
Name: Pennisi Gianfranco
File: main.c
Info: Mediante un PWM y un potenciometro, tal que el nivel de tension ingresado genere
una señal de frecuencia proporcional mediante un timer

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32_ub_lcd_2x16.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "adc.h"

/*Define*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t PrescalerValue = 0;
#define MAX_ADC 4095.0
char voltaje[4];
char frecuencia[4];
float tension=0;
int hertz=0;
int period=0;
uint16_t CCR1_Val = 0;
int32_t adc_tension;
uint32_t time_ms=0;

/*Definicion y Estructura de funciones */
void PreescaleerINIT(void)
{
	 /* -----------------------------------------------------------------------
						Tim1_Clock
			f= -----------------------------------------------
				(Tim_Period+1) * (PrescalerValue+1)
					 	 168MHz
			F Hz=---------------------
					(Tim_Period+1)*(419+1)
			Tim_Period=399 -> para 1khz
			Tim_Period=199 -> para 2khz
			Tim_Period=132 -> para 3khz

	  	    Note:
	  	     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	  	     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	  	     function to update SystemCoreClock variable value. Otherwise, any configuration
	  	     based on this variable will be incorrect.
	  	  ----------------------------------------------------------------------- */


	  	  /* Compute the prescaler value */
	  	  PrescalerValue = (uint16_t) ((SystemCoreClock ) / 400000) - 1;

	  	  /* Time base configuration */
	  	  TIM_TimeBaseStructure.TIM_Period = period;		// con TIM1 a 168MHz Prescaler = 419 (daria 400.000 KHz)

	  	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_CenterAligned1;

	  	  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	  	  /* PWM1 Mode configuration: Channel1 : para TIM1 es PE9 */
	  	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  	  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	  	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  	  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	  	  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	  	  TIM_ARRPreloadConfig(TIM1, ENABLE);

	  	  /* TIM4 enable counter */
	  	  TIM_Cmd(TIM1, ENABLE);
	  	  TIM_CtrlPWMOutputs(TIM1, ENABLE);

}

void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* GPIOE clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* GPIOE Configuration: TIM1 CH1 (PE9) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Connect TIM4 pins to AF2 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
}

void Salida(void)
{
	time_ms++;
	if(time_ms%1000==0)
	{
		UB_LCD_2x16_String(0,0,"TDII PWM");
		adc_tension=adc_leer_cuentas();
		tension=3*adc_tension;
		tension=tension/MAX_ADC;
		hertz=1000*tension;
		int FrecuenciaCort=0;
		UB_LCD_2x16_Clear();
		sprintf(voltaje,"%1.2f",tension);
		sprintf(frecuencia,"%d",hertz);
		UB_LCD_2x16_String(3,1,voltaje);
		UB_LCD_2x16_String(0,1,"V:");
		UB_LCD_2x16_String(8,1,"H:");
		UB_LCD_2x16_String(10,1,frecuencia);

		if(hertz<1)
		{
			TIM_Cmd(TIM1, DISABLE);
		}
		if (hertz!=FrecuenciaCort)
		{
			FrecuenciaCort=hertz;
			TIM_Cmd(TIM4, DISABLE);
			period=(168000000)/(hertz*420)-1;
			CCR1_Val = period/2;
			PreescaleerINIT();
		}
	}

}
int main(void)
{

	SystemInit();
	UB_LCD_2x16_Init();
	TIM_Config();
	SysTick_Config(SystemCoreClock / 1000);
	adc_inicializar();

  while (1)
  {
  }
}

/*
******************************************************************************
Name: Pennisi Gianfranco
Brief: Producir una señal de 250Hz y mediante el pulsador azul de la placa lograr ir aumentandole el ciclo de trabajo


******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32_ub_lcd_2x16.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdio.h>
#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "string.h"

/*Define*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t PrescalerValue = 0;
uint16_t CCR1_Val = 0;
double voltaje=0;
char volt[4];
uint32_t time_ms=0;

/*Definicion y Estructura de funciones */
void inicializar_leds(void)
{
	GPIO_InitTypeDef GPIO_Init_Pins;



	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_Init_Pins.GPIO_Pin= GPIO_Pin_12;

	GPIO_Init_Pins.GPIO_Mode=GPIO_Mode_OUT ;

	GPIO_Init_Pins.GPIO_Speed= GPIO_Speed_100MHz;

	GPIO_Init_Pins.GPIO_OType= GPIO_OType_PP ;

	GPIO_Init_Pins.GPIO_PuPd= GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD,&GPIO_Init_Pins);

}

void PreescaleerINIT(void)
{
	 /* -----------------------------------------------------------------------
	  	    AYUDA::::::TIM4 Configuration: generate 4 PWM signals with 4 different duty cycles.

	  	    In this example TIM4 input clock (TIM4CLK) is set to 2 * APB1 clock (PCLK1),
	  	    since APB1 prescaler is different from 1.
	  	      TIM4CLK = 2 * PCLK1
	  	      PCLK1 = HCLK / 4
	  	      => TIM4CLK = HCLK / 2 = SystemCoreClock /2  (ver archivo Excel Clock Config de ST)

	  	    To get TIM4 counter clock at 200 kHz, the prescaler is computed as follows:
	  	       Prescaler = (TIM4CLK / TIM4 counter clock) - 1
	  	       Prescaler = ((SystemCoreClock /2) /200 KHz) - 1

	  	    To get TIM3 output clock at 500 Hz, the period (ARR)) is computed as follows:
	  	       ARR = (TIM4 counter clock / TIM4 output clock) - 1
	  	           = 399

			Otro método:

			 				1
			f=	----------------------------------------------		Donde Tim4_Clock= PCLK1 x 2 (ver Clock_Config.XLS)
				(Tim_Period+1) * (PrescalerValue+1)/Tim4_Clock				en este ejemplo es 84 MHz

			o lo que es lo mismo:

						Tim4_Clock
			f= -----------------------------------------------
				(Tim_Period+1) * (PrescalerValue+1)
					 	 84MHz
			250 Hz=---------------------
					(Tim_Period+1)*(419+1)
			Tim_Period=799
	  	    Note:
	  	     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	  	     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	  	     function to update SystemCoreClock variable value. Otherwise, any configuration
	  	     based on this variable will be incorrect.
	  	  ----------------------------------------------------------------------- */


	  	  /* Compute the prescaler value */
	  	  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 200000) - 1;

	  	  /* Time base configuration */
	  	  TIM_TimeBaseStructure.TIM_Period = 799;		// con TIM4 a 84MHz Prescaler = 419 (daria 200.000 KHz)
	  	  	  	  	  	  	  	  	  	  	  	  	    // y TIM_Period 799 --> 250 Hz para la salida de este timer
	  	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_CenterAligned1;

	  	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	  	  /* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
	  	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  	  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	  	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  	  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	  	  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	  	  TIM_ARRPreloadConfig(TIM4, ENABLE);

	  	  /* TIM4 enable counter */
	  	  TIM_Cmd(TIM4, ENABLE);
}

void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* GPIOC Configuration: TIM4 CH1 (PD12) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Connect TIM4 pins to AF2 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
}

void inicializarPinEN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);



	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //Entrada
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;


	    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Salida(void)
{
	time_ms++;
	if(time_ms%1000==0)
	{
		sprintf(volt,"%1.2f",voltaje);
		UB_LCD_2x16_String(0,1,"Volt:");
		UB_LCD_2x16_String(6,1,volt);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
		{
			// TIM4 enable counter
			TIM_Cmd(TIM4, DISABLE);
			if (CCR1_Val<800)
			{
				CCR1_Val+=134;
		  		voltaje+=0.5;
			}
			else
			{
				CCR1_Val=0;
				voltaje=0;
			}
			TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
			TIM_OC1Init(TIM4, &TIM_OCInitStructure);

			// TIM4 enable counter
			TIM_Cmd(TIM4, ENABLE);

			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
		 }
	}

}
int main(void)
{

	SystemInit();
	UB_LCD_2x16_Init();
	inicializarPinEN();
	inicializar_leds();
	TIM_Config();
	PreescaleerINIT();
	SysTick_Config(SystemCoreClock / 1000);

	UB_LCD_2x16_String(0,0,"TDII PWM");

  while (1)
  {
  }
}

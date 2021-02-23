/*
 * Name: Pennisi Gianfranco
 * Brief: Trabajo Practico Numero 5 P1
 * Version:1.0
 *
 *
 */
//Librerias
#include "stm32f4xx.h"
#include "stm32_ub_lcd_4x20.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"


//Variables Globales-Inicializacion de funciones

uint32_t time_ms=0;
void InicializarPines(void);
void Despachador(void);
void Cronometro(void);
void TimerInterrupcion(void);
void EXTILine6_Config(void);
void EXTILine8_Config(void);

int suma=0;
int tiempo=0;
int boton=0;
int aux=0;
int variable=100000;
int ContadorCronometro=0;
char Pantalla[16];
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
uint16_t PrescalerValue = 0;
EXTI_InitTypeDef   EXTI_InitStructure;


//MAIN

int main(void)
{
	SystemInit();
	TimerInterrupcion();
	UB_LCD_4x20_Init();
	InicializarPines();

	SysTick_Config(SystemCoreClock/1000);
	EXTILine6_Config();
	EXTILine8_Config();

	while (1)
	{
	}
}

//Funciones
void InicializarPines(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Salidas de la placa a la proto //

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);

}

void EXTILine6_Config(void)
{
	  GPIO_InitTypeDef   GPIO_InitStructure;
	  NVIC_InitTypeDef   NVIC_InitStructure;

	  /* Enable GPIOC clock */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  /* Configure PC6 pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Connect EXTI Line6 to PC6 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);

	  /* Configure EXTI Line6 */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  /* Enable and set EXTI Line1 Interrupt to the lower priority */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

void EXTILine8_Config(void)
{
	  GPIO_InitTypeDef   GPIO_InitStructure;
	  NVIC_InitTypeDef   NVIC_InitStructure;

	  /* Enable GPIOC clock */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  /* Configure PC8 pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Connect EXTI Line8 to PC8 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);

	  /* Configure EXTI Line8 */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  /* Enable and set EXTI Line8 Interrupt to the lower priority */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

void TimerInterrupcion(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

		/* Habilita el reloj de TIM3 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		/* Habilita la interrupción global TIM3 */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		//*******************************************************************************
		// Resumen de formulas a utilizar:

		// TIM3 counter clock (TIM3CK_CNT)= TIM3CLK/(PrescalerValue+1)

		// PrescalerValue = ((TIM3CLK) / TIM3CK_CNT) - 1;
		// PrescalerValue = ((SystemCoreClock / 2) / TIM3CK_CNT) - 1;

		// Tiempo entre interrupciones (Tei) = TIM_Period * PrescalerValue + 1
		//												    ------------------
		//										                 TIM3CLK
		//*******************************************************************************
		/*
		 * TIM3CLK = 84 MHz
		 * TIMP_Period = 21000
		 * PrescalerValue = 999
		 * Tei= 250 mseg
		 */

		/* Configuración de Base de Tiempo */
		TIM_TimeBaseStructure.TIM_Period = 21000;   //Número entre 0x0000 y 0xFFFF
		TIM_TimeBaseStructure.TIM_Prescaler = 0;		//no modificar
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;  //no modificar
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		/* Computa el valor del Prescaler */
		/* Este valor puede ser modificado ON FLY, una vez que el contador está activo */
		PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 84000) - 1; //Prescaler = 999

		/* Configuracón de Prescaler */
		TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

		/* Habilita la interrupción a TIM3 */
		TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

		/* Habilita el contador TIM3 */
		TIM_Cmd(TIM3, ENABLE);

}
void RefrescarDisplay(void)
{
	UB_LCD_4x20_Clear();
	UB_LCD_4x20_String(0,0,"TDII Despachador");
	sprintf(Pantalla,"Seg:%d Ind:%d",tiempo,suma);
	UB_LCD_4x20_String(0,1,Pantalla);
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) != RESET || EXTI_GetITStatus(EXTI_Line8) != RESET )
	{
		while(variable!=0)
		{
			variable--;
		}
		variable=100000;
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
			{
				boton=1;
				if(boton!=aux)
				{
					suma= suma + boton;
				}
				aux=boton;
			}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
			{
				boton=2;
				if(boton!=aux)
				{
					suma= suma + boton;
				}
				aux=boton;
			}
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
			{
				boton=3;
				if(boton!=aux)
				{
					suma= suma + boton;
				}
				aux=boton;
			}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
			{
				boton=4;
				if(boton!=aux)
				{
					suma= suma + boton;
				}
				aux=boton;
			}

		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)) || !(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)))
		{
			aux=0;
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line8);
	}

}
void Cronometro()
{
	ContadorCronometro=ContadorCronometro+1;
	if(ContadorCronometro%1000==0)
	{
		tiempo=tiempo+1;
	}

}

/*
******************************************************************************
Prueba de seteo de alarma con rtc

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32_ub_lcd_4x20.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "tm_stm32f4_rtc.h"
/* Private macro */
/* Private variables */
int boton=0;
int aux=0;
int flecha=0;
int enter=0;
int atras=0;
char Fecha[20];
char Reloj[20];
char Alarma[20];
int i=0;
EXTI_InitTypeDef   EXTI_InitStructure;
int variable=100000;
TM_RTC_Time_t DataTime;
/* Private function prototypes */
void EXTILine8_Config(void);
void EXTILine6_Config(void);
void InicializarPines(void);
void InicializarHora(void);
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

	SystemInit();
  InicializarPines();
  EXTILine6_Config();
  EXTILine8_Config();
  UB_LCD_4x20_Init();
  while(!TM_RTC_Init(TM_RTC_ClockSource_Internal))
  {
	  UB_LCD_4x20_String(0,0,"Mala inicializacion");
	  UB_LCD_4x20_String(0,1,"Reinicie Micro");
  }
  UB_LCD_4x20_Clear();
  InicializarHora();
  UB_LCD_4x20_Clear();
  TM_RTC_Interrupts(TM_RTC_Int_1s);
  while (1)
  {
	TM_RTC_GetDateTime(&DataTime,TM_RTC_Format_BIN);
	sprintf(Reloj,"%02d/%02d/%02d %02d:%02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes,DataTime.seconds);
	UB_LCD_4x20_String(0,0,Reloj);
	UB_LCD_4x20_String(0,2,Alarma);
  }
}

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
				flecha= flecha + 1;
			}
			aux=boton;
		}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
		{
			boton=2;
			if(boton!=aux)
			{
				enter= enter + 1;
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
				flecha= flecha - 1;
			}
			aux=boton;
		}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
		{
			boton=4;
			if(boton!=aux)
			{
				atras= atras + 1;
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

void InicializarHora(void)
{
	flecha=0;
	enter=0;
	atras=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%d/%d/%d %d:%d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,0,Fecha);
		DataTime.date= flecha;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%d/%d/%d %d:%d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,0,Fecha);
		DataTime.month= flecha;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%d/%d/%d %d:%d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,0,Fecha);
		DataTime.year= flecha;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%d/%d/%d %d:%d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,0,Fecha);
		DataTime.hours= flecha;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%d/%d/%d %d:%d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,0,Fecha);
		DataTime.minutes = flecha;
	}
	flecha=0;
	enter=0;

	UB_LCD_4x20_Clear();
	while(enter!=1)
	{
		sprintf(Fecha,"%d",DataTime.day);
		UB_LCD_4x20_String(0,0,"Ingrese dia de la");
		UB_LCD_4x20_String(0,1,"semana entre 1 y 7");
		UB_LCD_4x20_String(0,2,Fecha);
		DataTime.day= flecha;
	}
	flecha=0;
	enter=0;
	TM_RTC_SetDateTime(&DataTime,TM_RTC_Format_BIN);
}

void TM_RTC_RequestHandler()
{
	TM_RTC_GetDateTime(&DataTime,TM_RTC_Format_BIN);
	/* If user needs this function, then they should be defined separatelly in your project */
	if(DataTime.minutes==0 && DataTime.seconds==0)
	{
		i=i+1;
		sprintf(Alarma,"Alarma andando %d",i);
	}
	else if(DataTime.minutes==15 && DataTime.seconds==0)
	{
		i=i+1;
		sprintf(Alarma,"Alarma andando %d",i);
	}
	else if(DataTime.minutes==30 && DataTime.seconds==0)
	{
		i=i+1;
		sprintf(Alarma,"Alarma andando %d",i);
	}
	else if(DataTime.minutes==45 && DataTime.seconds==0)
	{
		i=i+1;
		sprintf(Alarma,"Alarma andando %d",i);
	}
}

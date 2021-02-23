/*
 * Name: Pennisi Gianfranco
 * Brief: Trabajo Practico Numero 7
 * Version:1.0
 *
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_exti.h"
#include "stm32_ub_lcd_2x16.h"
#include "misc.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
/******************************************************/
void PasajedeCadenasUSART (USART_TypeDef* uart, char *msj);
void EXTILine0_Config(void);
void EXTI0_IRQHandler(void);
void USART2_Configuration(void);
void GPIO_Configuration(void);
void RCC_Configuration(void);
/*******************************************************/
EXTI_InitTypeDef   EXTI_InitStructure;
char Cadena[30] = "TDII & Electrónica lo mejor!! ";
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	UB_LCD_2x16_Init();
	USART2_Configuration();
	EXTILine0_Config();
  while (1)
  {
  }
}

void RCC_Configuration(void)
{
  /* --------------------------- System Clocks Configuration -----------------*/
  /* USART3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}

void EXTILine0_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line6 to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line0 Interrupt to the lower priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	UB_LCD_2x16_String(0,0,"Tiempo en uS:");
	double tiempo=0;
	int tiempo_unid=0;
	int tiempo_decimas=0;
	char tiempos[8];
	uint32_t cycles = 0;

	if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
	{
		DWT->CTRL |= 1 ; // enable the counter
		DWT->CYCCNT = 0; // reset the counter

		PasajedeCadenasUSART(USART2,Cadena);
		cycles = DWT->CYCCNT;
		cycles--; /*
			 We subtract the cycle used to transfer CYCCNT content to cycles variable */
		tiempo = (double)cycles * 1.0/168.0;
		tiempo_unid= (int)tiempo;
		tiempo_decimas=(tiempo-(double)tiempo_unid)*10.0;
		//************************************************************
		/* PRESENTACION EN DISPLAY */
		sprintf(tiempos," %d,%d",tiempo_unid,tiempo_decimas);
		UB_LCD_2x16_String(7,1,tiempos);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
void USART2_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configurada como sigue:
        - BaudRate = 9600 baud
        - Largo de palabra = 8 Bits
        - Un Bit de stop
        - Paridad par
        - COntrol de flujo por hardware deshabilitado (RTS and CTS signals)
        - Recepcion y transmision habilitadas
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);

  USART_Cmd(USART2, ENABLE);
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*-------------------------- GPIO Configuration ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect USART pins to AF */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
}

void PasajedeCadenasUSART (USART_TypeDef* uart, char *msj)
{
	int n= strlen(msj);
	int i=0;

		while(i<n)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, *msj);

			msj++;
			i++;
		}
}



/*
 * Name: Pennisi Gianfranco
 * Brief: Trabajo Practico Numero 6 P2
 * Version:1.0
 *
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "string.h"
#include "stm32_ub_lcd_2x16.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"

/*************************************************************************/
void USART2_Configuration(void);
void Mayusculas(char *msj);
void GPIO_Configuration(void);
void RCC_Configuration(void);
void RecibirCadenaUSART(USART_TypeDef* uart, char *msj);
void PasajedeCadenasUSART (USART_TypeDef* uart, char *msj);
/*************************************************************************/

char Recibido[40];

int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	USART2_Configuration();
	UB_LCD_2x16_Init();


  while (1)
  {
	  if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
	     {
		  	  RecibirCadenaUSART(USART2,Recibido);
		  	  PasajedeCadenasUSART(USART2,Recibido);
	      }

	  if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
	  {
		  UB_LCD_2x16_String(0,0,"Mensaje Enviado");
	  }
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
void USART2_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;


  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configurada como sigue:
        - BaudRate = 9600 baud
        - Largo de palabra = 8 Bits
        - Un Bit de stop
        - Sin paridad
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

void Mayusculas(char *msj)
{
	switch (*msj)
	{
		case 'a':*msj=toupper(*msj);break;
		case 'e':*msj=toupper(*msj);break;
		case 'i':*msj=toupper(*msj);break;
		case 'o':*msj=toupper(*msj);break;
		case 'u':*msj=toupper(*msj);break;
		default:
			break;
	}

	return;
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
			Mayusculas(msj);
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, *msj);

			msj++;
			i++;
		}


}
void RecibirCadenaUSART(USART_TypeDef* uart, char *msj)
{

	while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	{
		*msj= USART_ReceiveData(USART2);
		msj++;
	}


}


/*
 * Name: Pennisi Gianfranco
 * Brief: Despachador de tareas
 * Version:1.0
 *
 *
 */
//Librerias
#include "stm32f4xx.h"
#include "stm32_ub_lcd_2x16.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>
#include <string.h>


//Variables Globales-Inicializacion de funciones

uint32_t time_ms=0;
void InicializarPines(void);
void Despachador(void);
void PrimerTarea(void);
void SegundaTarea(void);
void TercerTarea(int *tiempo);

int suma=0;
int tiempo=0;
int aux=0;
int boton=0;
int contadorRefresco=1;
int contadorSegundos=1;
int contadorBoton=1;
//MAIN
int main(void)
{
  UB_LCD_2x16_Init();
  InicializarPines();
  SystemInit();
  SysTick_Config(SystemCoreClock/100);

  while (1)
  {

  }
}

//Funciones

void Despachador(void)
{
	PrimerTarea();
	SegundaTarea();
	TercerTarea(&tiempo);
}
void InicializarPines(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Salidas de la proto a la placa//

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);



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


}
void PrimerTarea(void)
{
	contadorRefresco++;
	char Suma[4];
	if(contadorRefresco%25==0)
	{
		UB_LCD_2x16_String(0,0,"TDII Despachador");
		UB_LCD_2x16_String(0,1,"Seg:");
		UB_LCD_2x16_String(8,1,"Ind:");
		sprintf(Suma,"%d",suma);
		UB_LCD_2x16_String(12,1,Suma);
	}


}

void SegundaTarea(void)
{
	contadorBoton++;

	if(contadorBoton%10==0)
	{
		int columna0=0;
		int columna1=0;

		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_SetBits(GPIOA, GPIO_Pin_8);

		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
		{
			columna0=1;
		}
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
		{
			columna1=1;
		}
		if (columna0 || columna1)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_9);
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
			{
				contadorBoton=1;
				boton=1;

					if(boton!=aux)
					{
						suma= suma + boton;
					}

				aux=boton;
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
			{
				contadorBoton=1;
				boton=2;
				if(boton!=aux)
				{
					suma= suma + boton;
				}

				aux=boton;

			}
			GPIO_ResetBits(GPIOC, GPIO_Pin_9);
			GPIO_SetBits(GPIOA, GPIO_Pin_8);
			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
			{
				contadorBoton=1;
				boton=3;
				if(boton!=aux)
				{
					suma= suma + boton;
				}

				aux=boton;

			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
			{
				contadorBoton=1;
				boton=4;
				if(boton!=aux)
				{
					suma= suma + boton;
				}

				aux=boton;
			}
		}
		contadorBoton=1;
		boton=0;
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)) &&!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)))
		{
			aux=0;
		}

	}

}

void TercerTarea(int *tiempo)
{
	contadorSegundos++;
	if(contadorSegundos%100==0)
	{
		char Tiempo[3];
		*tiempo= *tiempo+1;
		sprintf(Tiempo,"%d",*tiempo);
		UB_LCD_2x16_String(4,1,Tiempo);
	}

}

/*
 * Name: Pennisi Gianfranco
 * Brief: Cronometro mediante la funcion systick y un pulsador mostrandolo por un LCD
 * Date: Junio 2020 Version 1
 *
 *
 *
 */
#include "stm32f4xx.h"
#include "stm32_ub_lcd_2x16.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_it.h"

static __IO uint32_t TimingDelay;
uint32_t time_ms=0;
static void Delay(__IO uint32_t nTime);
int Antirebote ();

int main(void)
{
  SystemInit();
  SysTick_Config(SystemCoreClock/1000);
  UB_LCD_2x16_Init();
  double tiempo=0.00;
  char Tiempo[10];

  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //Pin del Boton azul
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //Entrada
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //Velocidad de respuesta del toque
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //Estado de inicializacion como no tocado

  GPIO_Init(GPIOA, &GPIO_InitStructure);


  UB_LCD_2x16_String(0,0,"Cronometro TDII");
  UB_LCD_2x16_String(0,1,"Seg:");
  int Pulsador=0;

  while (1)
  {
	  Pulsador=Antirebote();
	  if(Pulsador!=0)
	  {
		  Pulsador=0;
		  while(Pulsador==0)
		  {
		  	sprintf(Tiempo,"%1.2lf",tiempo);
		  	UB_LCD_2x16_String(4,1,Tiempo);
		  	tiempo=tiempo+0.01;
		  	Delay(10);
		  	Pulsador=Antirebote();
		  }

	  }
	  Delay(25);
  }
}


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0)
  {
    TimingDelay--;
  }
}

int Antirebote ()
{
	int contador=0;
	int Corto=10;

	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
	{
		Delay(9);
		contador++;
	}

	if(contador<Corto)
	{
		return 0;
	}
	if(contador>Corto)
	{
		return 1;
	}

	return 0;
}

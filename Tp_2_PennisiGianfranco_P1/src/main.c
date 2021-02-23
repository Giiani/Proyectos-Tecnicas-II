/*
File: Main.c
Autor: Pennisi Gianfranco
Descripcion: Uso de un potenciometro y el adc
para poder simular una entrada anologica de voltaje y temperatura
y mostrarla a traves de un display
*/


#include "stm32f4xx.h"
#include "stm32_ub_lcd_2x16.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include <stm32f4xx_adc.h>
#include <stm32f4xx_rcc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_ADC 4095.0

int32_t adc_tension;
int32_t adc_temperatura;
static __IO uint32_t TimingDelay;
uint32_t time_ms=0;
static void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

int main(void)
{

  SystemInit();
  SysTick_Config(SystemCoreClock / 1000);
  adc_inicializarPC1();
  adc_inicializarPC2();
  UB_LCD_2x16_Init();

  char voltajes[9];
  char Temperatura[7];
  float tension=0;
  float grados=0;

  while (1)
  {
	  Delay(10000);
	  adc_tension=adc_leer_cuentasPC1();
	  tension=3*adc_tension;
	  tension=tension/MAX_ADC;
	  adc_temperatura=adc_leer_cuentasPC2();
	  grados=3*adc_temperatura;
	  grados=grados/MAX_ADC;
	  grados=grados*100.0;
	  sprintf(voltajes,"%1.2f",tension);
	  sprintf(Temperatura,"%1.0f",grados);

	  UB_LCD_2x16_Clear();
	  UB_LCD_2x16_String(0,0,voltajes);
	  UB_LCD_2x16_String(5,0,"Volt");
	  UB_LCD_2x16_String(0,1,Temperatura);
	  UB_LCD_2x16_String(5,1,"Grados");


  }
}

/**
  * @brief  Aplica un retardo de tiempo real
  * @param  nTime: es el retardo en mseg
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0)
  {
	  TimingDelay_Decrement();
  }

}

/**
  * @brief  Decrementa el valor de TimingDelay. Esta función
  * es llamada por la rutina de atención del SysTick
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0)
  {
    TimingDelay--;
  }
}

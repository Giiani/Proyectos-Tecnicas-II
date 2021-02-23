/*
 * Proyecto de funcionamiento del sensor de humedad de suelo
 * conectado al pin PC1 y a 3V
 *
 */
/* Includes */
#include "stm32f4xx.h"
#include "adc.h"
#include "stm32_ub_lcd_4x20.h"
#include "stdio.h"
/* Private macro */
/* Private variables */
static __IO uint32_t TimingDelay;
uint32_t time_ms=0;
uint16_t adc_cuentas;
/* Private function prototypes */
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
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
	adc_inicializar();
	UB_LCD_4x20_Init();
	SysTick_Config(SystemCoreClock / 1000);
  int humedadTierra=0;
  char texto[20];

  while (1)
  {

	Delay(5000000);
	UB_LCD_4x20_Clear();
	adc_cuentas=adc_leer_cuentas();
	if(adc_cuentas < 1750)
	{
			humedadTierra = 100;
	}
	else if(adc_cuentas > 3100)
	{
		humedadTierra = 0;
	}
	else
	{
		humedadTierra= (adc_cuentas-3100)/(-14);
	}

	sprintf(texto,"Humedad Tierra%%:%d", humedadTierra);
	UB_LCD_4x20_String(0,0,texto);
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
  * @brief  Decrementa el valor de TimingDelay. Esta funci�n
  * es llamada por la rutina de atenci�n del SysTick
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

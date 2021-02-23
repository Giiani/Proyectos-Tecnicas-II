/* Includes */
#include "stm32f4xx.h"
#include "stm32_ub_lcd_4x20.h"
#include "DHT.h"
#include "stdio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_rcc.h"

/* Private macro */
/* Private variables */
float Temperature, Humidity;
DHT_DataTypedef DHT11_Data;
char texto[20];
uint32_t Timing;
uint32_t time_ms;
/* Private function prototypes */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t time_ms);
/* Private functions */

int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock/1000);
	UB_LCD_4x20_Init();
	TIM2_Start();

	while(1)
	{
		DHT_GetData(&DHT11_Data);
		Temperature = DHT11_Data.Temperature;
		Humidity = DHT11_Data.Humidity;
		sprintf(texto,"T:%.2f H:%.2f ",Temperature,Humidity);
		UB_LCD_4x20_String(0,0,texto);
		Delay(3000);
	}
}

void Delay(__IO uint32_t time_ms)
{
  Timing = time_ms;

  while(Timing != 0);
}
void TimingDelay_Decrement(void)
{
  if (Timing != 0)
  {
    Timing--;
  }
}

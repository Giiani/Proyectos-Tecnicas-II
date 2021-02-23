/**
  ******************************************************************************
  * @file    main.c
  * @author  Pennisi Gianfranco
  * @version
  * @info TP1 Punto 1
  * @brief
  * LEDS USER
  * led3 user naranja puerto I/O PD13
  * led4 user verde puerto I/O PD12
  * led5 user rojo puerto I/O PD14
  * led6 user azul puerto I/O PD15


  ******************************************************************************
 **/


#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>



int main(void)
{
	void delay(uint32_t tiempo);
	uint32_t aumentar(int Loop,uint32_t N);
	int Loop=0;
	uint32_t N=0xf0000;

	GPIO_InitTypeDef GPIO_Init_Pins;



	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_Init_Pins.GPIO_Pin= GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;

	GPIO_Init_Pins.GPIO_Mode=GPIO_Mode_OUT ;

	GPIO_Init_Pins.GPIO_Speed= GPIO_Speed_100MHz;

	GPIO_Init_Pins.GPIO_OType= GPIO_OType_PP ;

	GPIO_Init_Pins.GPIO_PuPd= GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD,&GPIO_Init_Pins);


    while(1)
    {

    	GPIO_SetBits(GPIOD,GPIO_Pin_12);
    	delay(N);
    	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
    	delay(N);

    	GPIO_SetBits(GPIOD,GPIO_Pin_13);
    	delay(N);
    	GPIO_ResetBits(GPIOD,GPIO_Pin_13);
    	delay(N);

    	GPIO_SetBits(GPIOD,GPIO_Pin_14);
    	delay(N);
    	GPIO_ResetBits(GPIOD,GPIO_Pin_14);
    	delay(N);

    	GPIO_SetBits(GPIOD,GPIO_Pin_15);
    	delay(N);
    	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
    	delay(N);

    	Loop++;
    	N=aumentar(Loop,N);

    }
}


void delay(uint32_t tiempo)
{

	while(--tiempo);

}

uint32_t aumentar(int Loop,uint32_t N)
{
	if(Loop<15)
	{
	    N=N-0xf000;
	}

	if(Loop==15)
	{
	   while(1)
	   {
	    	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	    	GPIO_SetBits(GPIOD,GPIO_Pin_13);
	    	GPIO_SetBits(GPIOD,GPIO_Pin_14);
	    	GPIO_SetBits(GPIOD,GPIO_Pin_15);
	    	delay(0xf000000);
	    	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	    	GPIO_ResetBits(GPIOD,GPIO_Pin_13);
	    	GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	    	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	    	delay(0xf0);

	    }
	  }

	return N;
}

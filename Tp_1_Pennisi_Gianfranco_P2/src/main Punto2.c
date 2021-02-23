/**
  ******************************************************************************
  * @file    main.c
  * @author  Pennisi Gianfranco
  * @version
  * @info TP1 Punto 2
  * @brief
  * LEDS USER
  * led3 user naranja puerto I/O PD13
  * led4 user verde puerto I/O PD12
  * led5 user rojo puerto I/O PD14
  * led6 user azul puerto I/O PD15


  ******************************************************************************
 **/

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "string.h"


int main(void)
	{
	void Delay(uint32_t nCount); //Función básica para generar un delay.
	uint32_t Achicar(uint32_t N);
	void ElegirLED(int Valor, int N); //Funcion para elegir LED a prender
	int Valor=0;
    GPIO_InitTypeDef GPIO_InitStructure; //Estructura para la inicialización de los GPIO

    //Habilitación de la senal de reloj para el periferico GPIOD (el mismo esta conectado al bus
    //AHB1), el cual es el puerto al cual estan conectado los LEDs de usuario

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    //Se configuran los pines PD12, 13, 14 y 15

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &GPIO_InitStructure);	//Se aplica la configuración definida anteriormente
											//al puerto D

	//Habilitación de la señal de reloj para el periferico GPIOA (el mismo esta conectado al bus
	//AHB1), el cual es el puerto al cual esta conectado el pulsador de usuario.

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    //Ahora se configura el pin PA0, el cual es el pin al cual esta conectado el pulsador
	//denominado "User" (pulsador azul)

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //Entrada
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Nota: Se puede comprobar que si en lugar de:

	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL

	//Se utiliza:

	//GPIO_PuPd_UP		Es como si el pulsador estuviese siempre pulsado
	//GPIO_PuPd_DOWN	Sin efecto

	//Esto se debe a que el pin P0 del GPIOA tiene un pull-down en el mismo kit, por lo que
	//configrar el pin sin pull-up / down o con pull-down es lo mismo. Distinto es claro esta
	//configurarlo con pull-up que hace que siempre este un 1 presente en el pin

    GPIO_Init(GPIOA, &GPIO_InitStructure);	//Se aplica la configuración definida anteriormente
    										//al puerto A
    uint32_t N=0x600000;
    uint32_t Pregunta=0x600000;

    while (1)
    	{

        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
        	{
        	Delay(Pregunta);
        			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
        					{
        						Valor++;
        						ElegirLED(Valor,N);
        	        			if(Valor>3)
        	        			{
        	        				Valor=0;
        	        			}

        	        		}
        			else
        			{
        				ElegirLED(Valor,N);
        			}


        	}

        N=Achicar(N);

		//Se apagan todos los LEDs simultaneamente

		GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

		Delay(0x0FFFFF);
    	}
	}


void Delay(uint32_t nCount)
	{
	while(nCount--)
		{
		}
	}
uint32_t Achicar(uint32_t N)
{
	uint32_t menor;
	menor= N*0.9;
	return menor;
}

void ElegirLED(int Valor, int N)
{
	if(Valor==0)
	   {
	       GPIO_SetBits(GPIOD, GPIO_Pin_12);	//PD13 encendido
	       Delay(N);
	       GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	       Delay(N);
	   }
	if(Valor==1)
	   {
	        GPIO_SetBits(GPIOD, GPIO_Pin_13);	//PD14 encendido
	        Delay(N);
	        GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	        Delay(N);
	    }
	 if(Valor==2)
	    {
	        GPIO_SetBits(GPIOD, GPIO_Pin_14);	//PD15 encendido
	        Delay(N);
	        GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	        Delay(N);
	    }
	  if(Valor==3)
	    {
	        GPIO_SetBits(GPIOD, GPIO_Pin_15);	//PD12 encendido
	        Delay(N);
	        GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	        Delay(N);
	     }
}

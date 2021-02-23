/*
 * Autor: Pennisi Gianfranco
 * Version: 1.0 ANDANDO
 * Brief: Menu para proyecto, uso de lcd y botones
 * Pines a utilizar:
 * PB8-> LCD_RS
 * PE5-> LCD_E
 * PE4-> LCD_DB4
 * PE6-> LCD_DB5
 * PE2-> LCD_DB6
 * PC13-> LCD_DB7
 * PC6 PIN 2 PLACA EXP
 * PC8 PIN 4 PLACA EXP
 * PC9 PIN 3 PLACA EXP
 * PA8 PIN 1 PLACA EXP
 *
 *
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


/* Private macro */
/* Private variables */
int boton=0;
int aux=0;
int flecha=0;
int enter=0;
int atras=0;
double temperatura=0;
double humedadaire=0;
double humedadtierra=0;
uint32_t Timing;
uint32_t time_ms;
EXTI_InitTypeDef   EXTI_InitStructure;
int variable=100000;


/* Private function prototypes */
void PantallaStanBy(void);
void EXTILine8_Config(void);
void EXTILine7_Config(void);
void InicializarPines(void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t time_ms);
void MenudeOpciones(void);

/* Private functions */


int main(void)
{
	SystemInit();
	UB_LCD_4x20_Init();
	InicializarPines();

	SysTick_Config(SystemCoreClock/1000);
	EXTILine7_Config();
	EXTILine8_Config();
	enter=0;
	atras=0;
	flecha=0;



  while (1)
  {
	  MenudeOpciones();
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

void EXTILine7_Config(void)
{
	  GPIO_InitTypeDef   GPIO_InitStructure;
	  NVIC_InitTypeDef   NVIC_InitStructure;

	  /* Enable GPIOC clock */
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  /* Configure PC6 pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  /* Connect EXTI Line6 to PC6 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);

	  /* Configure EXTI Line6 */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
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
	if(EXTI_GetITStatus(EXTI_Line7) != RESET || EXTI_GetITStatus(EXTI_Line8) != RESET )
	{
		while(variable!=0)
		{
			variable--;
		}
		variable=100000;
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7))
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
		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7))
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
		if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)) || !(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7)))
		{
			aux=0;
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line8);
	}

}

/*Pantalla que se mostrara siempre en el dispositivo
 * en esta se mostrara la hora del reloj
 * asi como tambien la temperatura del habitaculo asi como la humedad de tierra y aire
 * refrescandota cada media hora
 * De esta pantalla se saldra tocando algun boton lo que hara que se ingrese al menu
 */
void PantallaStanBy(void)
{
	char TemperaturaLCD[6];
	char HumedadAireLCD[6];
	char HumedadTierraLCD[6];


	sprintf(TemperaturaLCD,"%2.2lf",temperatura);
	sprintf(HumedadAireLCD,"%2.2lf",humedadaire);
	sprintf(HumedadTierraLCD,"%2.2lf",humedadtierra);

	UB_LCD_4x20_String(0,0,"Hora: ");
	UB_LCD_4x20_String(0,1,"Temperatura:");
	UB_LCD_4x20_String(13,1,TemperaturaLCD);
	UB_LCD_4x20_String(0,2,"Humedad A:");
	UB_LCD_4x20_String(11,2,HumedadAireLCD);
	UB_LCD_4x20_String(0,3,"Humedad T:");
	UB_LCD_4x20_String(11,3,HumedadTierraLCD);

}

/*
 * Menu de opciones que apareceria en pantalla cuando se oprima algun boton
 * En este menu serviria para poder setear las variables de los distintos valores
 * como los horarios de las luces, los rangos de temperatura y humedad que se deseen mantener durante el
 * habitaculo
 */
void MenudeOpciones(void)
{
	UB_LCD_4x20_Clear();

	if(flecha==0 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==0 && enter!=1)
		{
			UB_LCD_4x20_String(0,0,"> Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				UB_LCD_4x20_String(0,0,"Horarios Luces:");
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(0,0,"> Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		atras=0;
		enter=0;
	}
	if(flecha==-1 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-1 && enter!=1)
		{
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(0,1,"> Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				UB_LCD_4x20_String(0,0,"Rango Temperatura:");
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(0,1,"> Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		atras=0;
		enter=0;
	}
	if(flecha==-2 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-2 && enter!=1)
		{
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(0,2,"> Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				UB_LCD_4x20_String(0,0,"%Humedad Aire:");
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(0,2,"> Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");
		}
		atras=0;
		enter=0;
	}
	if(flecha==-3 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-3 && enter!=1)
		{
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(0,3,"> Humedad Tierra");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				UB_LCD_4x20_String(0,0,"%Humedad Tierra:");
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(2,0,"Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(0,3,"> Humedad Tierra");
		}
		atras=0;
		enter=0;
	}

	if(flecha==-4 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-4 && enter!=1)
		{
			UB_LCD_4x20_String(0,0,"> SD");
			UB_LCD_4x20_String(2,1,"Estado actual");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				UB_LCD_4x20_String(0,0,"Enviar Datos?");
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(0,0,"> SD");
			UB_LCD_4x20_String(2,1,"Estado actual");
		}
		atras=0;
		enter=0;
	}
	if(flecha==-5 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-5 && enter!=1)
		{
			UB_LCD_4x20_String(2,0,"SD");
			UB_LCD_4x20_String(0,1,"> Estado actual");
		}
		if(enter==1)
		{
			UB_LCD_4x20_Clear();
			while(atras==0)
			{
				PantallaStanBy();
			}
		}
		else if(atras==1)
		{
			UB_LCD_4x20_Clear();
			UB_LCD_4x20_String(2,0,"SD");
			UB_LCD_4x20_String(0,1,"> Estado actual");
		}
		atras=0;
		enter=0;
	}

	if(flecha<-5)
	{
		flecha=0;
	}
	if(flecha>0)
	{
		flecha=-5;
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

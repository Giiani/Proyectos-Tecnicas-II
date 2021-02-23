/*
******************************************************************************
PRUEBA 1.2 ANDANDO BIEN TODO PERRI
INTERRUPCIONES DE 10 MIN


Sensor Humedad Tierra -> PC1
Sensor DHT22 -> PA1

SD
CS ->PB12
MOSI ->PB15
MISO ->PB14
SCK ->PB13

LCD
RS-> PB8
E -> PE5
D4 ->PE4
D5 ->PE6
D6 ->PC13
D7 ->PE2

TECLADO
  PD7 PIN 2 PLACA EXP
  PC8 PIN 4 PLACA EXP
  PC9 PIN 3 PLACA EXP
  PA8 PIN 1 PLACA EXP

BLUETOOH
PC6 -> TX
PC7 -> RX

SALIDAS A RELAY Y AVISO PARA AGUA
LUCES PE7
VENTILADORES PE9
BOMBA PE11



F_Wr escribe sobre algo creado, si no existe no lo crea
F_WR_NEW crea archivo nuevo y si existe lo sobrescribe
******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32_ub_lcd_4x20.h"
#include "stm32_ub_fatfs.h"
#include "stm32_ub_sdcard.h"
#include "adc.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "tm_stm32f4_rtc.h"
#include "DHT.h"
#include "HC06.h"

/* Private macro */
/* Private variables */
int boton=0;
int aux=0;
int flecha=0;
int enter=0;
int atras=0;
int HacerAccion=0;
int variable=100000;
int humedadTierra=0;
int AlarmaTierra=100;
int HoraAlarmaInicio=0;
int HoraAlarmaFinal=0;
int MinutosAlarmaInicio=0;
int MinutosAlarmaFinal=0;
int EnviarDatos=0;

unsigned long int contador=0;

float Temperature=0;
float Humidity=0;
float AlarmaTemperatura=0;
float AlarmaAire=0;

uint16_t adc_cuentas;
uint32_t Timing;
uint32_t time_ms;

char Fecha[20];
char TemperaturaLCD[20];
char HumedadAireLCD[20];
char HumedadTierraLCD[20];
char HorarioLCD[20];
char TemperaturaSD[20];
char HumedadAireSD[20];
char HumedadTierraSD[20];
char HorarioSD[20];
char FechaSD[20];
char TemperaturaBLUE[20];
char HumedadAireBLUE[20];
char HumedadTierraBLUE[20];


EXTI_InitTypeDef   EXTI_InitStructure;
TM_RTC_Time_t DataTime;
DHT_DataTypedef DHT11_Data;
FIL Datos;

/* Private function prototypes */
void sensar(void);
void EXTILine8_Config(void);
void EXTILine7_Config(void);
void InicializarPines(void);
void InicializarHora(void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t time_ms);
void escribirSD(void);
void inicializaSensores(void);
void PantallaStanBy(void);
void MenudeOpciones(void);
void HumedadTierra(void);
void InicializarPinesSalida(void);
void HorariosLuces(void);
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
	SysTick_Config(SystemCoreClock/1000);
	InicializarPines();
	InicializarPinesSalida();
	EXTILine7_Config();
	EXTILine8_Config();
	UB_LCD_4x20_Init();
	TIM2_Start();
	UB_Fatfs_Init();
	adc_inicializar();
	inicializarBluetooth();

	while(!TM_RTC_Init(TM_RTC_ClockSource_Internal))
	{
		UB_LCD_4x20_String(0,0,"Mala inicializacion");
		UB_LCD_4x20_String(0,1,"Reinicie Micro");
	}

	UB_LCD_4x20_Clear();
	InicializarHora();
	UB_LCD_4x20_Clear();
	TM_RTC_Interrupts(TM_RTC_Int_1s);
	inicializaSensores();
	escribirSD();


	while (1)
	{
		MenudeOpciones();
	}
}

void PantallaStanBy(void)
{

	TM_RTC_GetDateTime(&DataTime,TM_RTC_Format_BIN);
	sprintf(HorarioLCD,"Hora: %02d:%02d:%02d",DataTime.hours,DataTime.minutes,DataTime.seconds);
	sprintf(TemperaturaLCD,"Temperatura:%2.2f C",Temperature);
	sprintf(HumedadAireLCD,"HumedadA:%2.2f%% g/m3",Humidity);
	sprintf(HumedadTierraLCD,"HumedadT:%02d %%",humedadTierra);


	UB_LCD_4x20_String(0,0,HorarioLCD);
	UB_LCD_4x20_String(0,1,TemperaturaLCD);
	UB_LCD_4x20_String(0,2,HumedadAireLCD);
	UB_LCD_4x20_String(0,3,HumedadTierraLCD);

}

void InicializarPinesSalida(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Salidas de la placa a la proto //

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void inicializaSensores(void)
{
	int n=0;
	UB_LCD_4x20_String(0,0,"Inicializando ");
	UB_LCD_4x20_String(0,1,"Sensores");
	HumedadTierra();
	while(Humidity==0 && Temperature==0)
	{
		DHT_GetData(&DHT11_Data);
		Temperature = DHT11_Data.Temperature;
		Humidity = DHT11_Data.Humidity;
		Delay(3000);
		n=n+1;
		if(n==10)
		{
			UB_LCD_4x20_String(0,2,"Error de Inicio");
			UB_LCD_4x20_String(0,3,"Reinicie");
		}
	}

}
void sensar(void)
{
	contador=contador+1;
	if(contador==600000)
	{
			HumedadTierra();

			if(humedadTierra<=AlarmaTierra)
			{
				GPIO_SetBits(GPIOE,GPIO_Pin_11);
			}
			else
			{
				GPIO_ResetBits(GPIOE,GPIO_Pin_11);
			}

			if(Temperature>=AlarmaTemperatura || Humidity>=AlarmaAire)
			{
				GPIO_SetBits(GPIOE,GPIO_Pin_9);
			}
			else
			{
				GPIO_ResetBits(GPIOE,GPIO_Pin_9);
			}

			if(EnviarDatos==1)
			{
				escribirSD();
			}
			contador=0;
	}
	if(contador%5000==0)
	{
		sprintf(TemperaturaBLUE,"Temperatura:%2.2f C",Temperature);
		sprintf(HumedadAireBLUE,"HumedadA:%2.2f%% g/m3",Humidity);
		sprintf(HumedadTierraBLUE,"HumedadT:%02d %%",humedadTierra);


		blue_Puts(TemperaturaBLUE);
		blue_Putc('\n');
		blue_Puts(HumedadAireBLUE);
		blue_Putc('\n');
		blue_Puts(HumedadTierraBLUE);
		blue_Putc('\n');

		DHT_GetData(&DHT11_Data);
		Temperature = DHT11_Data.Temperature;
		Humidity = DHT11_Data.Humidity;
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

void HumedadTierra(void)
{
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
	char AlarmaTemperaturaC[20];
	char AlarmaAireC[20];
	char AlarmaTierraC[20];
	char EnviarDatosC[20];

	if(flecha==0 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==0 && enter!=1)
		{
			UB_LCD_4x20_String(0,0,"> Horario");
			UB_LCD_4x20_String(2,1,"Temperatura");
			UB_LCD_4x20_String(2,2,"Humedad Aire");
			UB_LCD_4x20_String(2,3,"Humedad Tierra");

			if(enter==1)
			{
				UB_LCD_4x20_Clear();
				while(atras==0)
				{
					UB_LCD_4x20_String(0,0,"Horarios Luces:");
					HorariosLuces();
				}
				UB_LCD_4x20_Clear();
			}

			atras=0;
			enter=0;
		}
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

			if(enter==1)
			{
				enter=0;
				flecha=0;
				UB_LCD_4x20_Clear();
				while(atras==0)
				{
					UB_LCD_4x20_String(0,0,"Rango Temperatura:");
					while(enter!=1)
					{
						AlarmaTemperatura=flecha;
						sprintf(AlarmaTemperaturaC,"Temp= %3.0f C",AlarmaTemperatura);
						UB_LCD_4x20_String(0,2,AlarmaTemperaturaC);
						if(flecha>=101)
						{
							flecha=0;
						}
						else if(flecha<=-1)
						{
							flecha=100;
						}
					}
				}
				flecha=-1;
				UB_LCD_4x20_Clear();
			}

			atras=0;
			enter=0;
		}
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

			if(enter==1)
			{
				UB_LCD_4x20_Clear();
				enter=0;
				flecha=0;
				while(atras==0)
				{
					UB_LCD_4x20_String(0,0,"%Humedad Aire:");
					while(enter!=1)
					{
						AlarmaAire=flecha;
						sprintf(AlarmaAireC,"HumA= %3.0f %% g/m3",AlarmaAire);
						UB_LCD_4x20_String(0,2,AlarmaAireC);
						if(flecha>=101)
						{
							flecha=0;
						}
						else if(flecha<=-1)
						{
							flecha=100;
						}
					}
				}
				flecha=-2;
				UB_LCD_4x20_Clear();
			}

			atras=0;
			enter=0;
		}
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

			if(enter==1)
			{
				UB_LCD_4x20_Clear();
				enter=0;
				flecha=0;
				while(atras==0)
				{
					UB_LCD_4x20_String(0,0,"%Humedad Tierra:");
					while(enter!=1)
					{
						AlarmaTierra=flecha;
						sprintf(AlarmaTierraC,"HumT= %3d %%",AlarmaTierra);
						UB_LCD_4x20_String(0,2,AlarmaTierraC);
						if(flecha>=101)
						{
							flecha=0;
						}
						else if(flecha<=-1)
						{
							flecha=100;
						}
					}
				}
				flecha=-3;
				UB_LCD_4x20_Clear();
			}
			atras=0;
			enter=0;
		}
	}

	if(flecha==-4 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-4 && enter!=1)
		{
			UB_LCD_4x20_String(0,0,"> SD");
			UB_LCD_4x20_String(2,1,"Estado actual");

			if(enter==1)
			{
				UB_LCD_4x20_Clear();
				enter=0;
				flecha=0;
				while(atras==0)
				{
					UB_LCD_4x20_String(0,0,"Enviar Datos a SD?");
					while(enter!=1)
					{
						EnviarDatos=flecha;
						sprintf(EnviarDatosC,"Decision= %d",EnviarDatos);
						UB_LCD_4x20_String(0,1,"1:SI 0:NO");
						UB_LCD_4x20_String(0,2,EnviarDatosC);
						if(flecha>=2)
						{
							flecha=0;
						}
						else if(flecha<=-1)
						{
							flecha=1;
						}
					}
				}
				flecha=-4;
				UB_LCD_4x20_Clear();
			}
			atras=0;
			enter=0;
		}
	}
	if(flecha==-5 && atras==0)
	{
		UB_LCD_4x20_Clear();
		while(flecha==-5 && enter!=1)
		{
			UB_LCD_4x20_String(2,0,"SD");
			UB_LCD_4x20_String(0,1,"> Estado actual");

			if(enter==1)
			{
				UB_LCD_4x20_Clear();
				while(atras==0)
				{
					PantallaStanBy();
					blue_Puts("Hola");
				}
				UB_LCD_4x20_Clear();
			}

			atras=0;
			enter=0;
		}
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

void HorariosLuces(void)
{
	flecha=0;
	enter=0;
	char HoraInicio[20];
	char HoraFinal[20];

	while(enter!=1)
	{
		HoraAlarmaInicio=flecha;
		sprintf(HoraInicio,"Inicio: %02d:%02d",HoraAlarmaInicio,MinutosAlarmaInicio);
		sprintf(HoraFinal,"Final: %02d:%02d",HoraAlarmaFinal,MinutosAlarmaFinal);
		UB_LCD_4x20_String(0,1,HoraInicio);
		UB_LCD_4x20_String(0,2,HoraFinal);

		if(flecha>24)
		{
			flecha=0;
		}
		else if(flecha<0)
		{
			flecha=23;
		}
	}

	enter=0;
	flecha=0;

	while(enter!=1)
	{
		MinutosAlarmaInicio=flecha;
		sprintf(HoraInicio,"Inicio: %02d:%02d",HoraAlarmaInicio,MinutosAlarmaInicio);
		sprintf(HoraFinal,"Final: %02d:%02d",HoraAlarmaFinal,MinutosAlarmaFinal);
		UB_LCD_4x20_String(0,1,HoraInicio);
		UB_LCD_4x20_String(0,2,HoraFinal);

		if(flecha>59)
		{
			flecha=0;
		}
		else if(flecha<0)
		{
			flecha=59;
		}
	}

	flecha=0;
	enter=0;

	while(enter!=1)
	{
		HoraAlarmaFinal=flecha;
		sprintf(HoraInicio,"Inicio: %02d:%02d",HoraAlarmaInicio,MinutosAlarmaInicio);
		sprintf(HoraFinal,"Final: %02d:%02d",HoraAlarmaFinal,MinutosAlarmaFinal);
		UB_LCD_4x20_String(0,1,HoraInicio);
		UB_LCD_4x20_String(0,2,HoraFinal);

		if(flecha>24)
		{
			flecha=0;
		}
		else if(flecha<0)
		{
			flecha=23;
		}
	}

	flecha=0;
	enter=0;

	while(enter!=1)
	{
		MinutosAlarmaFinal=flecha;
		sprintf(HoraInicio,"Inicio: %02d:%02d",HoraAlarmaInicio,MinutosAlarmaInicio);
		sprintf(HoraFinal,"Final: %02d:%02d",HoraAlarmaFinal,MinutosAlarmaFinal);
		UB_LCD_4x20_String(0,1,HoraInicio);
		UB_LCD_4x20_String(0,2,HoraFinal);

		if(flecha>59)
		{
			flecha=0;
		}
		else if(flecha<0)
		{
			flecha=59;
		}
	}

	enter=0;
	flecha=0;
	atras=1;
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
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  /* Connect EXTI Line6 to PC6 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);

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
				while(variable!=0)
				{
					variable--;
				}
				variable=100000;
			}
			aux=boton;
		}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
		{
			boton=2;
			if(boton!=aux)
			{
				enter= enter + 1;
				while(variable!=0)
				{
					variable--;
				}
				variable=100000;
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
				while(variable!=0)
				{
					variable--;
				}
				variable=100000;
			}
			aux=boton;
		}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
		{
			boton=4;
			if(boton!=aux)
			{
				atras= atras + 1;
				while(variable!=0)
				{
					variable--;
				}
				variable=100000;
			}
			aux=boton;
		}

		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)) || !(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7)))
		{
			aux=0;
		}
		EXTI_ClearITPendingBit(EXTI_Line7);
		EXTI_ClearITPendingBit(EXTI_Line8);
	}

}

void InicializarHora(void)
{
	flecha=0;
	enter=0;
	atras=0;

	UB_LCD_4x20_String(0,0,"Inicialice hora");

	while(enter!=1)
	{
		sprintf(Fecha,"%02d/%02d/%02d %02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,1,Fecha);
		DataTime.date= flecha;
		if(flecha>=32)
			flecha=1;
		else if(flecha<=0)
			flecha=31;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%02d/%02d/%02d %02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,1,Fecha);
		DataTime.month= flecha;
		if(flecha>=13)
			flecha=1;
		else if(flecha<=0)
			flecha=12;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%02d/%02d/%02d %02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,1,Fecha);
		DataTime.year= flecha;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%02d/%02d/%02d %02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,1,Fecha);
		DataTime.hours= flecha;
		if(flecha>=24)
			flecha=0;
		else if(flecha<=-1)
			flecha=23;
	}
	flecha=0;
	enter=0;

	while(enter!=1)
	{
		sprintf(Fecha,"%02d/%02d/%02d %02d:%02d",DataTime.date,DataTime.month,DataTime.year,DataTime.hours,DataTime.minutes);
		UB_LCD_4x20_String(0,1,Fecha);
		DataTime.minutes = flecha;
		if(flecha>=60)
			flecha=0;
		else if(flecha<=-1)
			flecha=59;
	}
	flecha=0;
	enter=0;

	UB_LCD_4x20_Clear();
	while(enter!=1)
	{
		sprintf(Fecha,"%d",DataTime.day);
		UB_LCD_4x20_String(0,0,"Ingrese dia de la");
		UB_LCD_4x20_String(0,1,"semana entre 1 y 7");
		UB_LCD_4x20_String(0,2,Fecha);
		DataTime.day= flecha;

		if(flecha>=8)
			flecha=1;
		else if(flecha<=0)
			flecha=7;
	}
	flecha=0;
	enter=0;
	TM_RTC_SetDateTime(&DataTime,TM_RTC_Format_BIN);
}

void escribirSD(void)
{
	TM_RTC_GetDateTime(&DataTime,TM_RTC_Format_BIN);
	sprintf(FechaSD,"Dia: %02d:%02d:%02d \n",DataTime.date,DataTime.month,DataTime.year);
	sprintf(HorarioSD,"Hora: %02d:%02d\n",DataTime.hours,DataTime.minutes);
	sprintf(TemperaturaSD,"Temperatura:%2.2f ºC\n",Temperature);
	sprintf(HumedadAireSD,"HumedadA:%2.2f %%g/m3\n",Humidity);
	sprintf(HumedadTierraSD,"HumedadT:%02d %%\n",humedadTierra);

	// Comprueba si la SD esta insertada
	if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK)
	{
		// Media mounten
		if(UB_Fatfs_Mount(MMC_0)==FATFS_OK)
		{
			//Crea nuevo archivo para escribir
			if(UB_Fatfs_OpenFile(&Datos,"0:/Datos.txt", F_WR_NEW)==FATFS_OK)
			{
				// Escribe en el archivo
				UB_Fatfs_WriteString(&Datos,FechaSD);
				UB_Fatfs_WriteString(&Datos,HorarioSD);
				UB_Fatfs_WriteString(&Datos,TemperaturaSD);
				UB_Fatfs_WriteString(&Datos,HumedadAireSD);
				UB_Fatfs_WriteString(&Datos,HumedadTierraSD);
				UB_Fatfs_WriteString(&Datos,"\n");

				// Cierra el archivo
				UB_Fatfs_CloseFile(&Datos);
			}
			// Media unmounten
			UB_Fatfs_UnMount(MMC_0);
		}
		else
		{
			UB_LCD_4x20_String(0,0,"NO SE ENCUENTRA");
			UB_LCD_4x20_String(0,1,"TARJETA");
		}
	}
}

void TM_RTC_RequestHandler()
{
	/* If user needs this function, then they should be defined separatelly in your project */
	if(DataTime.hours==HoraAlarmaInicio && DataTime.minutes==MinutosAlarmaInicio)
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_7);
	}
	else if(DataTime.hours==HoraAlarmaFinal && DataTime.minutes==MinutosAlarmaFinal)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_7);
	}
}

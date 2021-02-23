/************** MAKE CHANGES HERE ********************/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "DHT.h"

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM; uint8_t Presence = 0;
uint32_t TimingDelay=0;
uint8_t Response = 0;

void TIM2_IRQHandler (void);
void TIM2_Config(void);
void TIM2_Init(void);

void DHT_GetData (DHT_DataTypedef *DHT_Data);
uint8_t DHT_Read (void);
uint8_t DHT_Check_Response (void);
void DHT_Start (void);

void Set_Pin_Input (void);
void Set_Pin_Output (void);

void delay(uint32_t tiempo)
{
	/* Funcion delay()
	 * Realiza un retardo en funcion de la frecuencia de reloj del microcontrolador.
	 * Recibe como parametro el retraso, para este caso, en mS debido a la configuracion del Systick.
	 */
	TimingDelay = tiempo;
	while(TimingDelay!=0);

}

void Set_Pin_Output (void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType= GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_PuPd= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Set_Pin_Input (void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void DHT_Start (void)
{
	Set_Pin_Output ();  // set the pin as output
	//GPIO_WriteBit(GPIOA,GPIO_Pin_1,1);// pull the pin low
	//delay(1000);
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,0);
	delay(1200);  // >1ms delay
	//GPIO_WriteBit(GPIOA,GPIO_Pin_1,1);
	//delay(20);
	Set_Pin_Input();    // set as input
}

uint8_t DHT_Check_Response (void)
{
	delay(40);
	if (!(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)))
	{
		delay(80);
		if ((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)))
			Response = 1;
		else Response = -1;
	}
	while (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1));   // wait for the pin to go low

	return Response;
}

uint8_t DHT_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)));   // wait for the pin to go high
		delay(40);   // wait for 40 us
		if ((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))==0)   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else
			i|= (1<<(7-j));  // if the pin is high, write 1

		while ((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)));  // wait for the pin to go low
	}
	return i;
}

void DHT_GetData (DHT_DataTypedef *DHT_Data)
{
	uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
	uint16_t SUM;

	TIM_Cmd(TIM2, ENABLE);

    DHT_Start ();
	Presence = DHT_Check_Response ();
	Rh_byte1 = DHT_Read ();
	Rh_byte2 = DHT_Read ();
	Temp_byte1 = DHT_Read ();
	Temp_byte2 = DHT_Read ();
	SUM = DHT_Read();

	if (SUM == (Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))
	{
		DHT_Data->Temperature = ((Temp_byte1<<8)|Temp_byte2)/10.00;
		DHT_Data->Humidity = ((Rh_byte1<<8)|Rh_byte2)/10.00;
	}

	TIM_Cmd(TIM2, DISABLE);
}

void TIM2_Init(void)
{
	/*
	 *	TIMX_Init Configura los parametros del timer X.
	 *	Formulas:
	 *	TIM3CLK = 84MHz. Reloj del timer, para modiicarlo hay que cambiar el archivo system_stm32f4xx.c .
	 *	frecuencia. Frecuencia a la que cuenta el timer, este valor modifica el prescaler.
	 *	TIM_Period = (Tei*TIM3CLK)/(Prescalervalue+1). Tei es el valor de periodo de interrupcion deseado en segundos.
	 *
	 *	ATENCION: TIM_Period y PrescalerValue no deben superar el valor de 0xFFFF (65536) ya que son de 16bits.
	 */

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0; // Variable para el prescaler.
	uint32_t frecuencia = 10e6; // Frecuencia del contador a 10kHz. Tener cuidado de no cometer overflow en la variable PrescalerValue.
	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / frecuencia) - 1; //Conversion de frecuencia al valor de Prescaler.
	TIM_TimeBaseStructure.TIM_Period = 10; // 321.5uS (10e3 = 1 seg --> 3.215 = 321.5uS) de periodo.
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE); // habilitacion de las interrupciones por el timer 5.
	TIM_Cmd(TIM2, ENABLE); // Habilita el contador para el timer 5.

}

void TIM2_Config(void)
{
	/*
	 *	TIMX_Config habilita el relog y las interrupciones globales para el timer X
	 */

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Se habilita el reloj.

	/* Se habilitan las interrupciones globales para el timer X*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void TIM2_Start(void)
{
	/*
	 *	TIMX_Start deja el timer X ready to go.
	 */

	TIM2_Config(); // Configuracion del timer.
	TIM2_Init(); // Inicializacion del timer.
	TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQHandler (void)
{
	/* TIM2_IRQHandler rutina de interrupcion del timer 5.
	 *  Realiza un toogle en el led azul de la placa discovery.
	 */

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); // Se limpia la bandera de interrupcion.

		if (TimingDelay != 0)
		{
			TimingDelay--;
		}

	}
}




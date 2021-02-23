/**
    @file ADC.C
    @brief Activar los puertos PC1(ADC1) y PC2(ADC2) para leer entradas analogicas

    @author Pennisi Gianfranco
    @date 31/5/20
*/

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include "adc.h"
#include <stm32f4xx_adc.h>
#include <stm32f4xx_rcc.h>


/**
    @brief Inicializa los puertos para poder usarlos como entrada de señal
*/
void adc_inicializarPC1(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_InitTypeDef         ADC_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;
   
    /* Habilita la entrada de reloj del puerto C*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* PC1 para entrada analogica */
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 

    /* Activar ADC1 ----------------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* ADC Common Init -------------------------------------------------------*/
    ADC_CommonStructInit(&ADC_CommonInitStructure);
    ADC_CommonInitStructure.ADC_Mode                = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler           = ADC_Prescaler_Div4; // max 36 MHz segun datasheet
    ADC_CommonInitStructure.ADC_DMAAccessMode       = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay    = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC Init ---------------------------------------------------------------*/
    ADC_StructInit (&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode           = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode     = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion        = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Establecer la configuracion de conversion ------------------------------*/
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_1, 0);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_480Cycles);

    /* Poner en marcha ADC ----------------------------------------------------*/
    ADC_Cmd(ADC1, ENABLE);   
}

void adc_inicializarPC2(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_InitTypeDef         ADC_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;

    /* Habilita la entrada de reloj del puerto C*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* PC2 para entrada analogica */
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Activar ADC2 ----------------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    /* ADC Common Init -------------------------------------------------------*/
    ADC_CommonStructInit(&ADC_CommonInitStructure);
    ADC_CommonInitStructure.ADC_Mode                = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler           = ADC_Prescaler_Div4; // max 36 MHz segun datasheet
    ADC_CommonInitStructure.ADC_DMAAccessMode       = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay    = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC Init ---------------------------------------------------------------*/
    ADC_StructInit (&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode           = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode     = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion        = 1;
    ADC_Init(ADC2, &ADC_InitStructure);

    /* Establecer la configuracion de conversion ------------------------------*/
    ADC_InjectedSequencerLengthConfig(ADC2, 2);
    ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_2, 0);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_12, 2, ADC_SampleTime_480Cycles);

    /* Poner en marcha ADC ----------------------------------------------------*/
    ADC_Cmd(ADC2, ENABLE);
}

/*****************************************************************************/
/**
    @brief Leer tension
    @returns
*/
int32_t adc_leer_cuentasPC1(void)
{
    uint32_t valor_adc1;

   

    ADC_ClearFlag(ADC1,ADC_FLAG_JEOC);      // borrar flag de fin conversion
    
    ADC_SoftwareStartInjectedConv(ADC1);    // iniciar conversion

    while (ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC) == RESET); // Espera fin de conversion

    valor_adc1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1); // obtiene Valor A-D



    return valor_adc1;


}

int32_t adc_leer_cuentasPC2(void)
{
    uint32_t valor_adc2;



    ADC_ClearFlag(ADC2,ADC_FLAG_JEOC);      // borrar flag de fin conversion

    ADC_SoftwareStartInjectedConv(ADC2);    // iniciar conversion

    while (ADC_GetFlagStatus(ADC2,ADC_FLAG_JEOC) == RESET); // Espera fin de conversion

    valor_adc2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_2); // obtiene Valor A-D



    return valor_adc2;


}

/*****************************************************************************/

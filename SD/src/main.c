/*
******************************************************************************
CS PB12
MOSI PB15
MISO PB14
SCK PB13

F_Wr escribe sobre algo creado, si no existe no lo crea
F_WR_NEW crea archivo nuevo y si existe lo sobrescribe

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32_ub_fatfs.h"
#include "stm32_ub_lcd_4x20.h"
#include "stm32_ub_led.h"
#include "stm32_ub_sdcard.h"
#include "stdio.h"
/* Private macro */
/* Private variables */
FIL prueba;
char Anda[50];
/* Private function prototypes */
void escribirSD(void);
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
	UB_Fatfs_Init();
	UB_LCD_4x20_Init();
	UB_Led_Init();

	escribirSD();
  while (1)
  {
  }
}

void escribirSD(void)
{
	int i=0;

	// Comprueba si la SD esta insertada
	if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK)
	{
		// Media mounten
		if(UB_Fatfs_Mount(MMC_0)==FATFS_OK)
		{
			UB_Led_On(LED_BLUE);
			//Crea nuevo archivo para escribir
			if(UB_Fatfs_OpenFile(&prueba,"0:/Prueba2.txt", F_WR_NEW)==FATFS_OK)
			{
				UB_Led_On(LED_GREEN);
				// Escribe en el archivo
				UB_Fatfs_WriteString(&prueba,"\n");
				UB_Fatfs_WriteString(&prueba,"Anduvo\n");
				for(i=0;i<4;i++)
				{
					sprintf(Anda,"Esto es una prueba numero [%d] \n",i);
					UB_Fatfs_WriteString(&prueba,Anda);
				}
				UB_Fatfs_WriteString(&prueba,"\n");

				UB_LCD_4x20_String(0,0,"Anduvo");
				// Cierra el archivo
				UB_Fatfs_CloseFile(&prueba);
			}
			// Media unmounten
			UB_Fatfs_UnMount(MMC_0);
		}
		else
		{
			UB_LCD_4x20_String(0,0,"NO SE ENCUENTRA");
			UB_LCD_4x20_String(0,1,"TARJETA");
			UB_Led_On(LED_RED);
		}
	}

	UB_Led_Off(LED_GREEN);
	UB_Led_Off(LED_BLUE);
	UB_Led_Off(LED_RED);
}

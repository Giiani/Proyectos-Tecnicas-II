//             PB8  -> LCD_RS
//             PE5  -> LCD_E
//             PE4  -> LCD_DB4
//             PE6  -> LCD_DB5
//             PE2  -> LCD_DB6
//             PC13 -> LCD_DB7
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_lcd_4x20.h"



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_LCD_4x20_InitIO(void);
void P_LCD_4x20_PinLo(TLCD_NAME_t lcd_pin);
void P_LCD_4x20_PinHi(TLCD_NAME_t lcd_pin);
void P_LCD_4x20_Clk(void);
void P_LCD_4x20_InitSequenz(void);
void P_LCD_4x20_Cmd(uint8_t wert);
void P_LCD_4x20_Data(uint8_t wert);
void P_LCD_4x20_Cursor(uint8_t x, uint8_t y);
void P_LCD_4x20_Delay(volatile uint32_t nCount);
 


//--------------------------------------------------------------
// Definition aller Pins für das Display
// Reihenfolge wie bei TLCD_NAME_t
//
// Init : [Bit_SET,Bit_RESET]
//--------------------------------------------------------------
LCD_2X16_t LCD_4X20[] = {
 // Name   ,PORT , PIN       , CLOCK              , Init
  {TLCD_RS ,GPIOB,GPIO_Pin_8 ,RCC_AHB1Periph_GPIOB,Bit_RESET},
  {TLCD_E  ,GPIOE,GPIO_Pin_5 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D4 ,GPIOE,GPIO_Pin_4 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D5 ,GPIOE,GPIO_Pin_6 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D6 ,GPIOC,GPIO_Pin_13 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  {TLCD_D7 ,GPIOE,GPIO_Pin_2,RCC_AHB1Periph_GPIOE,Bit_RESET},
};



//--------------------------------------------------------------
// Init vom Text-LCDisplay
//--------------------------------------------------------------
void UB_LCD_4x20_Init(void)
{
  // init aller IO-Pins
  P_LCD_4x20_InitIO();
  // kleine Pause
  P_LCD_4x20_Delay(TLCD_INIT_PAUSE);
  // Init Sequenz starten
  P_LCD_4x20_InitSequenz();
  // LCD-Settings einstellen
  P_LCD_4x20_Cmd(TLCD_CMD_INIT_DISPLAY);
  P_LCD_4x20_Cmd(TLCD_CMD_ENTRY_MODE);
  // Display einschalten
  P_LCD_4x20_Cmd(TLCD_CMD_DISP_M1);
  // Display löschen
  P_LCD_4x20_Cmd(TLCD_CMD_CLEAR);
  // kleine Pause
  P_LCD_4x20_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// Löscht das Text-LCDisplay
//--------------------------------------------------------------
void UB_LCD_4x20_Clear(void)
{
  // Display löschen
  P_LCD_4x20_Cmd(TLCD_CMD_CLEAR);
  // kleine Pause
  P_LCD_4x20_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// Stellt einen Display Mode ein
// mode : [TLCD_OFF, TLCD_ON, TLCD_CURSOR, TLCD_BLINK]
//--------------------------------------------------------------
void UB_LCD_4x20_SetMode(TLCD_MODE_t mode)
{
  if(mode==TLCD_OFF) P_LCD_4x20_Cmd(TLCD_CMD_DISP_M0);
  if(mode==TLCD_ON) P_LCD_4x20_Cmd(TLCD_CMD_DISP_M1);
  if(mode==TLCD_CURSOR) P_LCD_4x20_Cmd(TLCD_CMD_DISP_M2);
  if(mode==TLCD_BLINK) P_LCD_4x20_Cmd(TLCD_CMD_DISP_M3);
}


//--------------------------------------------------------------
// Ausgabe von einem String auf dem Display an x,y Position
// x : 0 bis 15
// y : 0 bis 1
//--------------------------------------------------------------
void UB_LCD_4x20_String(uint8_t x, uint8_t y, char *ptr)
{
  // Cursor setzen
  P_LCD_4x20_Cursor(x,y);
  // kompletten String ausgeben
  while (*ptr != 0) {
    P_LCD_4x20_Data(*ptr);
    ptr++;
  }
}

//--------------------------------------------------------------
// Speichern eines Sonderzeichens im CG-RAM vom Display
// nr : 0 bis 7 (nummer der sonderzeichens)
// pixeldata : 8 bytes mit Pixeldaten für das Zeichen
//--------------------------------------------------------------
void UB_LCD_4x20_WriteCG(uint8_t nr, uint8_t *pixeldata)
{
  uint8_t n;

  if(nr>7) nr=7;

  nr=(nr<<3);
  nr|=0x40;
  P_LCD_4x20_Cmd(nr);
  for(n=0;n<8;n++) {
    P_LCD_4x20_Data(pixeldata[n]);
  }
}

//--------------------------------------------------------------
// Ausgabe von einem Sonderzeichen auf dem Display an x,y Position
// x : 0 bis 15
// y : 0 bis 1
// nr : 0 bis 7 (nummer der sonderzeichens)
//--------------------------------------------------------------
void UB_LCD_4x20_PrintCG(uint8_t x, uint8_t y, uint8_t nr)
{
  if(nr>7) nr=7;

  // Cursor setzen
  P_LCD_4x20_Cursor(x,y);
  P_LCD_4x20_Data(nr);
}

//--------------------------------------------------------------
// interne Funktion
// init aller IO-Pins
//--------------------------------------------------------------
void P_LCD_4x20_InitIO(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  TLCD_NAME_t lcd_pin;
  
  for(lcd_pin=0;lcd_pin<TLCD_ANZ;lcd_pin++) {
    // Clock Enable
    RCC_AHB1PeriphClockCmd(LCD_4X20[lcd_pin].TLCD_CLK, ENABLE);

    // Config als Digital-Ausgang
    GPIO_InitStructure.GPIO_Pin = LCD_4X20[lcd_pin].TLCD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_4X20[lcd_pin].TLCD_PORT, &GPIO_InitStructure);

    // Default Wert einstellen
    if(LCD_4X20[lcd_pin].TLCD_INIT==Bit_RESET) {
      P_LCD_4x20_PinLo(lcd_pin);
    }
    else {
      P_LCD_4x20_PinHi(lcd_pin);
    }
  }  
}


//--------------------------------------------------------------
// interne Funktion
// Pin auf Lo setzen
//--------------------------------------------------------------
void P_LCD_4x20_PinLo(TLCD_NAME_t lcd_pin)
{
  LCD_4X20[lcd_pin].TLCD_PORT->BSRRH = LCD_4X20[lcd_pin].TLCD_PIN;
}


//--------------------------------------------------------------
// interne Funktion
// Pin auf Hi setzen
//--------------------------------------------------------------
void P_LCD_4x20_PinHi(TLCD_NAME_t lcd_pin)
{
  LCD_4X20[lcd_pin].TLCD_PORT->BSRRL = LCD_4X20[lcd_pin].TLCD_PIN;
}


//--------------------------------------------------------------
// interne Funktion
// einen Clock Impuls ausgeben
//--------------------------------------------------------------
void P_LCD_4x20_Clk(void)
{
  // Pin-E auf Hi
  P_LCD_4x20_PinHi(TLCD_E);
  // kleine Pause
  P_LCD_4x20_Delay(TLCD_CLK_PAUSE);
  // Pin-E auf Lo
  P_LCD_4x20_PinLo(TLCD_E);
  // kleine Pause
  P_LCD_4x20_Delay(TLCD_CLK_PAUSE);
}


//--------------------------------------------------------------
// interne Funktion
// init Sequenz für das Display
//--------------------------------------------------------------
void P_LCD_4x20_InitSequenz(void)
{
  // Init Sequenz
  P_LCD_4x20_PinHi(TLCD_D4);
  P_LCD_4x20_PinHi(TLCD_D5);
  P_LCD_4x20_PinLo(TLCD_D6);
  P_LCD_4x20_PinLo(TLCD_D7);
  // Erster Init Impuls
  P_LCD_4x20_Clk();
  P_LCD_4x20_Delay(TLCD_PAUSE);
  // Zweiter Init Impuls
  P_LCD_4x20_Clk();
  P_LCD_4x20_Delay(TLCD_PAUSE);
  // Dritter Init Impuls
  P_LCD_4x20_Clk();
  P_LCD_4x20_Delay(TLCD_PAUSE);
  // LCD-Modus einstellen (4Bit-Mode)
  P_LCD_4x20_PinLo(TLCD_D4);
  P_LCD_4x20_PinHi(TLCD_D5);
  P_LCD_4x20_PinLo(TLCD_D6);
  P_LCD_4x20_PinLo(TLCD_D7);
  P_LCD_4x20_Clk();
  P_LCD_4x20_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// interne Funktion
// Kommando an das Display senden
//--------------------------------------------------------------
void P_LCD_4x20_Cmd(uint8_t wert)
{
  // RS=Lo (Command)
  P_LCD_4x20_PinLo(TLCD_RS);
  // Hi-Nibble ausgeben         
  if((wert&0x80)!=0) P_LCD_4x20_PinHi(TLCD_D7); else P_LCD_4x20_PinLo(TLCD_D7);
  if((wert&0x40)!=0) P_LCD_4x20_PinHi(TLCD_D6); else P_LCD_4x20_PinLo(TLCD_D6);
  if((wert&0x20)!=0) P_LCD_4x20_PinHi(TLCD_D5); else P_LCD_4x20_PinLo(TLCD_D5);
  if((wert&0x10)!=0) P_LCD_4x20_PinHi(TLCD_D4); else P_LCD_4x20_PinLo(TLCD_D4);
  P_LCD_4x20_Clk();
  // Lo-Nibble ausgeben         
  if((wert&0x08)!=0) P_LCD_4x20_PinHi(TLCD_D7); else P_LCD_4x20_PinLo(TLCD_D7);
  if((wert&0x04)!=0) P_LCD_4x20_PinHi(TLCD_D6); else P_LCD_4x20_PinLo(TLCD_D6);
  if((wert&0x02)!=0) P_LCD_4x20_PinHi(TLCD_D5); else P_LCD_4x20_PinLo(TLCD_D5);
  if((wert&0x01)!=0) P_LCD_4x20_PinHi(TLCD_D4); else P_LCD_4x20_PinLo(TLCD_D4);
  P_LCD_4x20_Clk();
}


//--------------------------------------------------------------
// interne Funktion
// Daten an das Display senden
//--------------------------------------------------------------
void P_LCD_4x20_Data(uint8_t wert)
{
  // RS=Hi (Data)
  P_LCD_4x20_PinHi(TLCD_RS);
  // Hi-Nibble ausgeben          
  if((wert&0x80)!=0) P_LCD_4x20_PinHi(TLCD_D7); else P_LCD_4x20_PinLo(TLCD_D7);
  if((wert&0x40)!=0) P_LCD_4x20_PinHi(TLCD_D6); else P_LCD_4x20_PinLo(TLCD_D6);
  if((wert&0x20)!=0) P_LCD_4x20_PinHi(TLCD_D5); else P_LCD_4x20_PinLo(TLCD_D5);
  if((wert&0x10)!=0) P_LCD_4x20_PinHi(TLCD_D4); else P_LCD_4x20_PinLo(TLCD_D4);
  P_LCD_4x20_Clk();
  // Lo-Nibble ausgeben        
  if((wert&0x08)!=0) P_LCD_4x20_PinHi(TLCD_D7); else P_LCD_4x20_PinLo(TLCD_D7);
  if((wert&0x04)!=0) P_LCD_4x20_PinHi(TLCD_D6); else P_LCD_4x20_PinLo(TLCD_D6);
  if((wert&0x02)!=0) P_LCD_4x20_PinHi(TLCD_D5); else P_LCD_4x20_PinLo(TLCD_D5);
  if((wert&0x01)!=0) P_LCD_4x20_PinHi(TLCD_D4); else P_LCD_4x20_PinLo(TLCD_D4);
  P_LCD_4x20_Clk();
}


//--------------------------------------------------------------
// interne Funktion
// Cursor auf x,y stellen

void P_LCD_4x20_Cursor(uint8_t x, uint8_t y)
{

if(x>=TLCD_MAXX) x=0;
if(y>=TLCD_MAXY) y=0;

if(y==0) P_LCD_4x20_Cmd( (1<<7) + x);
else if(y==1) P_LCD_4x20_Cmd( (1<<7) + 0x40 + x);
else if(y==2) P_LCD_4x20_Cmd( (1<<7) + 0x14 + x);
else if(y==3) P_LCD_4x20_Cmd( (1<<7) + 0x54 + x);
}
//--------------------------------------------------------------
// kleine Pause (ohne Timer)
//--------------------------------------------------------------
void P_LCD_4x20_Delay(volatile uint32_t nCount)
{
  while(nCount--)
  {
  }
}

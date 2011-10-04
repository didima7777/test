#ifndef  __DRV_SSD1303_H__
#define  __DRV_SSD1303_H__

#include "macros.h"
#include <avr/pgmspace.h>

#define DATA_PORT PORTA     //Порт данных
#define DATA_DDR DDRA       //Регистр данных

#define CONFIG_LCD1_PORT PORTG   //Порт управляющих выходов
#define CONFIG_LCD1_DDR DDRG     //Регистр управляющих выходов
#define CS 1
#define WR 0
#define RD 2

#define DISP 5 //включение питания

#define CONFIG_LCD2_PORT PORTB   //Порт управляющих выходов
#define CONFIG_LCD2_DDR DDRB     //Регистр управляющих выходов

#define RES 6
#define RS  7

#define X_OFFSET  0     // Смещение по X
#define XMAX      132   // Ширина
#define YMAX      64    // Высота
#define PAGEMAX   8     // Размер страницы 64/8 px

typedef enum
{
    FONT_1X = 0,  
    FONT_2X = 1,  
    FONT_4X = 2 
} OLEDFontSize;

typedef enum
{
    FONT_fine   = 1,  
    FONT_normal = 2   
} OLEDFontWeight;

typedef enum
  {
    e_Mosaic = 1
  } DrawEffects;

//#define pgm_read_byte(b) (*(unsigned char *)(b)) 

extern void LCD_Init (void);
extern void LCD_PutPixel (unsigned char X,unsigned char Y, unsigned char color);
extern void DrawCircle(unsigned char X, unsigned char Y ,unsigned char R, unsigned char color);
extern void DrawBitmap(unsigned short  *bmp, unsigned char X, unsigned char Y, unsigned char width, unsigned char height, unsigned char color);
extern void LCD_clear(unsigned char color);
extern void OLED_PutText (unsigned char X,unsigned char Y, OLEDFontSize size, OLEDFontWeight font_weight, unsigned char color, const char* STR);
extern void OLED_PutChar(unsigned char CHAR,unsigned char X,unsigned char Y, unsigned char color);
extern void OLED_PutChar_FONT2(unsigned char CHAR, unsigned char X, unsigned char Y, OLEDFontWeight font_weight, unsigned char color);
extern void OLED_PutChar_FONT4(unsigned char CHAR, unsigned char X, unsigned char Y, OLEDFontWeight font_weight, unsigned char color);
extern void Mosaic(unsigned short  *bmp, unsigned char X, unsigned char Y, unsigned char width, unsigned char height, unsigned char color);
extern void DrawBitmapWEffect(unsigned short  *bmp, unsigned short X, unsigned short Y, unsigned short width, unsigned short height, unsigned short color, DrawEffects Effect);
extern void DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
extern void LCD_clear_str(char page_start,char page_end,char x_start,char x_end,char color);
extern void OLED_PutChar_new(unsigned char CHAR, unsigned char X, unsigned char Y, unsigned char color);
extern void OLED_PutText_flash(unsigned char X,unsigned char Y, OLEDFontSize size, OLEDFontWeight font_weight,unsigned char color, const char* STR);

#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "driver_ssd1303.h"
#include "macros.h"
#include "fonts.h"

void LCD_WriteCommand(unsigned char data)
  {
    DATA_DDR = 0xFF;
    
    SetBit(CONFIG_LCD1_PORT, RD);
    ClrBit(CONFIG_LCD2_PORT, RS);
    ClrBit(CONFIG_LCD1_PORT, CS);
    ClrBit(CONFIG_LCD1_PORT, WR);
    
    DATA_PORT = data;
    
    SetBit(CONFIG_LCD1_PORT, CS);
    SetBit(CONFIG_LCD1_PORT, WR);
	SetBit(CONFIG_LCD2_PORT, RS);
  }

void LCD_WriteData(unsigned char data)
  {
    DATA_DDR = 0xFF;
    
    //SetBit(CONFIG_LCD1_PORT, RD);
    //SetBit(CONFIG_LCD2_PORT, RS);
    ClrBit(CONFIG_LCD1_PORT, CS);
    ClrBit(CONFIG_LCD1_PORT, WR);
    
    DATA_PORT = data;
    
    SetBit(CONFIG_LCD1_PORT, CS);
    SetBit(CONFIG_LCD1_PORT, WR);
  }

unsigned char LCD_read_data(unsigned char dummy)
  {
    unsigned char data_read;

    DATA_DDR = 0x00;

    ClrBit(CONFIG_LCD1_PORT, RD);
    //SetBit(CONFIG_LCD2_PORT, RS);
    ClrBit(CONFIG_LCD1_PORT, CS);
    //SetBit(CONFIG_LCD1_PORT, WR);

    __no_operation(); 
//    __no_operation();
//    __no_operation(); 

    data_read = PINA; 
    SetBit(CONFIG_LCD1_PORT, RD);

    if (dummy==1) 
      { 
        ClrBit(CONFIG_LCD1_PORT, CS); 
      }  
    else 
      { 
        SetBit(CONFIG_LCD1_PORT, CS); 
      } 

    DATA_DDR = 0xFF;

    return data_read;
  } 

void LCD_clear_str(char page_start,char page_end,char x_start,char x_end,char color)
  {
    unsigned char i,pages;    
    unsigned char adress=x_start;       

    for (pages=page_start; pages < page_end; pages++)           
	{                        
    LCD_WriteCommand(0xB0 + pages);                   
    LCD_WriteCommand((unsigned char)adress & 0x0F);   
    LCD_WriteCommand((unsigned char)((adress & 0xF0) >> 4) + 0x10);       

    for (i=x_start; i < x_end; i++)	         
      {
       LCD_WriteData(color);
      }
    }
           
  }

void LCD_clear(unsigned char color)
  {
    unsigned char i,pages;    
    unsigned char adress=X_OFFSET;       

    for (pages=0; pages < PAGEMAX; pages++)           
	{                        
    LCD_WriteCommand(0xB0 + pages);                   
    LCD_WriteCommand((unsigned char)adress & 0x0F);   
    LCD_WriteCommand((unsigned char)((adress & 0xF0) >> 4) + 0x10);       

    for (i=0; i < XMAX; i++)	         
      {
       LCD_WriteData(color);
      }
    }
           
  }

void LCD_PutPixel(unsigned char X,unsigned char Y, unsigned char color)
  {
    unsigned char data;     
    unsigned char adress=X_OFFSET;       

    if( (X>=XMAX) || (Y>=YMAX) )return;

    adress += X;                                                 
 
    LCD_WriteCommand(0xB0 + (Y/8));
    LCD_WriteCommand((unsigned char)adress & 0x0F);   
    LCD_WriteCommand((unsigned char)((adress & 0xF0) >> 4) + 0x10);       

    LCD_WriteCommand(0xE0); // вход в режим записи
    data = LCD_read_data(1);              //dummy read
    data = LCD_read_data(0);              //realy read
    LCD_WriteCommand(0xEE);  // выход из режимазаписи       

 
    if(color == 1) 
      data |= 0x01 << (Y%8);     
    else 
      data &= ~(0x01 << (Y%8));  
	
    LCD_WriteData(data);
  }

void LCD_Init(void)
  {
    DATA_DDR = 0xFF;
    CONFIG_LCD1_DDR |= 0x07;
    CONFIG_LCD1_PORT&= ~(1<<CS|1<<WR|1<<RD);

    CONFIG_LCD2_DDR |= (1<<RS)|(1<<RES);
    CONFIG_LCD2_PORT &=~((1<<RS)|(1<<RES));
    
    delay_ms(800);
    //SetBit(CONFIG__PORT, DISP);
    SetBit(CONFIG_LCD2_PORT, RES);

    LCD_WriteCommand(0xAE);
    LCD_WriteCommand(0x00);
    LCD_WriteCommand(0x10);
                           
    LCD_WriteCommand(0xAD);//dc-dc on/off
    LCD_WriteCommand(0x8B);

    LCD_WriteCommand(0xA8);//Set Multiplexratio
    LCD_WriteCommand(0x3F);

    LCD_WriteCommand(0x40);// Set Display Start Line 

    LCD_WriteCommand(0xA6);// Set Normal/Inverse Display

    LCD_WriteCommand(0xA0); //Set Segment Re-map **

    LCD_WriteCommand(0xC8); //Set COM Output Scan Direction  ** 

    LCD_WriteCommand(0xD3); //  Set Display Offset ** 
    LCD_WriteCommand(0x00);               

    LCD_WriteCommand(0xD8);//Set area colour mode on/off&low power dis mode
    LCD_WriteCommand(0x00);


    LCD_WriteCommand(0xDA); // Set COM pins hardware configuration 
    LCD_WriteCommand(0x12);

    LCD_WriteCommand(0x81); //Set Contrast Control Register **
    LCD_WriteCommand(0xff); //0x88

    LCD_WriteCommand(0x82); //Brightness for color banks 
    LCD_WriteCommand(0x9f); //0x80
                           
    LCD_WriteCommand(0xD5); //Set Disp Clk Divide Ratio/Oscill Frequency 
    LCD_WriteCommand(0x70);   

    LCD_WriteCommand(0xD9); //Set Pre-charge period*  
    LCD_WriteCommand(0x22);    
                               
    LCD_WriteCommand(0xA4); //Set Entire Display ON/OFF **  
    LCD_WriteCommand(0xAF); //Set Display ON/OFF ** 
    delay_ms(800);

    
    LCD_clear(0);     
    
  }

void DrawCircle(unsigned char X, unsigned char Y ,unsigned char R, unsigned char color)
  {
    int xc = 0, yc, p;

    yc=R;
    p = 3 - (R<<1);

    while (xc <= yc)  
    {
      LCD_PutPixel (X + xc, Y + yc, color);
      LCD_PutPixel (X + xc, Y - yc, color);
      LCD_PutPixel (X - xc, Y + yc, color);
      LCD_PutPixel (X - xc, Y - yc, color);
      LCD_PutPixel (X + yc, Y + xc, color);
      LCD_PutPixel (X + yc, Y - xc, color);
      LCD_PutPixel (X - yc, Y + xc, color);
      LCD_PutPixel (X - yc, Y - xc, color);
      if (p < 0)
        p += (xc++ << 2) + 6;
      else
        p += ((xc++ - yc--)<<2) + 10;
    }
  }

void DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) 
{
  int dy ;
  int dx ;
  int step_x, step_y, fraction;
  dy = y2 - y1;
  dx = x2 - x1;
  if (dy < 0) 
  {
    dy = -dy;
    step_y = -1;
  }
  else 
  {
    step_y = 1;
  }
  if (dx < 0)
  {
    dx = -dx;
    step_x = -1;
  }
  else
  {
    step_x = 1;
  }
  dy <<= 1;
  dx <<= 1;
  LCD_PutPixel(x1,y1,color);
  if (dx > dy)
  {
    fraction = dy - (dx >> 1); 
    while (x1 != x2)
    {
      if (fraction >= 0)
      {
        y1 += step_y;
        fraction -= dx;
      }
      x1 += step_x;
      fraction += dy;  
      LCD_PutPixel(x1,y1,color);
    }
  }
  else
  {
    fraction = dx - (dy >> 1);
    while (y1 != y2)
    {
      if (fraction >= 0)
      {
        x1 += step_x;
        fraction -= dy;
      }
      y1 += step_y;
      fraction += dx;
      LCD_PutPixel(x1,y1,color);
    }
  }
}

void DrawBitmap(unsigned short  *bmp, unsigned char X, unsigned char Y, unsigned char width, unsigned char height, unsigned char color)
{
  unsigned short i=0, j=0, k=0;
  for(j=height;j>0;j--)
  {
    for(i=0;i<width;i++)
    {
      if (bmp[k]==0) LCD_PutPixel(X+i,Y+j,1);
      k++;
    }   
  }
}

void Mosaic(unsigned short  *bmp, unsigned char X, unsigned char Y, unsigned char width, unsigned char height, unsigned char color)
  {
    unsigned short i=0, j=0, k=1;
    for(j=0;j<height/2;j++)
    {
      for(i=0;i<width;i++)
      {
        if (bmp[k]==0) LCD_PutPixel(X+i,Y+j,1);
        k++;
      }
      delay_ms(1);
    }
    k=sizeof(bmp);
    for(j=height;j>height/2;j--)
    {
      for(i=0;i<width;i++)
      {
        if (bmp[k]==0) LCD_PutPixel(X+i,Y+j,1);
        k--;
      }
      delay_ms(1);
    }
  }

void DrawBitmapWEffect(unsigned short  *bmp, unsigned short X, unsigned short Y, unsigned short width, unsigned short height, unsigned short color, DrawEffects Effect)
  {
    switch (Effect)
      {
      case e_Mosaic:
        Mosaic(bmp, X, Y, width, height, color); break;
      }
  }

void OLED_PutText (unsigned char X,unsigned char Y, OLEDFontSize size, OLEDFontWeight font_weight,
                   unsigned char color, const char* STR)
{       
 unsigned char i;
  
 unsigned char X1;
 unsigned char Y1;
        switch (size)
        {
          case FONT_1X:
            {
              for (i=0; STR[i]!=0;i++)
               {
			   	X1=121-X;
				Y1=56-Y;
                OLED_PutChar_new(STR[i],X1,Y1, color);
                X += 6; 

                if (X >= (XMAX-5)) 
                 {
                  X = 0;
                  Y += 8;
                 } 
               }
              break;
            }

         case FONT_2X:
            {
              for (i=0; STR[i]!=0;i++)
               {
                OLED_PutChar_FONT2(STR[i],X,Y, font_weight, color);

                if (font_weight==FONT_normal)
                 { 
                  X += 12;
                   if (X >= (XMAX-11))
                    {
                     X = 0;
                     Y += 16;
                    }
                 } 
                else if (font_weight==FONT_fine)
                 {
                  X += 6;
                  if (X >= (XMAX))
                   {
                    X = 0;
                    Y += 16;
                   }
                 }
                }  

              break;
            }

         case FONT_4X:
            {
              for (i=0; STR[i]!=0;i++)
               {
                OLED_PutChar_FONT4(STR[i],X,Y, font_weight, color);

                if (font_weight==FONT_normal)
                 {
                  X += 24;
                   if (X >= (XMAX-23))
                    {
                     X = 0;
                     Y += 32;
                    }
                 } 
                else if (font_weight==FONT_fine)
                 {
                  X += 12;
                   if (X >= (XMAX-11))
                    {
                     X = 0;
                     Y += 32;
                    }
                  }
                 }   

              break;
            }

          default:
	    	{
              break;
            }

         }
}

void OLED_PutText_flash(unsigned char X,unsigned char Y, OLEDFontSize size, OLEDFontWeight font_weight,
                   unsigned char color, const char* STR)
{       
 unsigned char i;
  
 unsigned char X1;
 unsigned char Y1;
        switch (size)
        {
          case FONT_1X:
            {
              for (i=0; pgm_read_byte(&STR[i])!=0;i++)
               {
			   	X1=121-X;
				Y1=56-Y;
                OLED_PutChar_new(pgm_read_byte(&STR[i]),X1,Y1, color);
                X += 6; 

                if (X >= (XMAX-5)) 
                 {
                  X = 0;
                  Y += 8;
                 } 
               }
              break;
            }

         case FONT_2X:
            {
              for (i=0; pgm_read_byte(&STR[i])!=0;i++)
               {
                OLED_PutChar_FONT2(pgm_read_byte(&STR[i]),X,Y, font_weight, color);

                if (font_weight==FONT_normal)
                 { 
                  X += 12;
                   if (X >= (XMAX-11))
                    {
                     X = 0;
                     Y += 16;
                    }
                 } 
                else if (font_weight==FONT_fine)
                 {
                  X += 6;
                  if (X >= (XMAX))
                   {
                    X = 0;
                    Y += 16;
                   }
                 }
                }  

              break;
            }

         case FONT_4X:
            {
              for (i=0; pgm_read_byte(&STR[i])!=0;i++)
               {
                OLED_PutChar_FONT4(pgm_read_byte(&STR[i]),X,Y, font_weight, color);

                if (font_weight==FONT_normal)
                 {
                  X += 24;
                   if (X >= (XMAX-23))
                    {
                     X = 0;
                     Y += 32;
                    }
                 } 
                else if (font_weight==FONT_fine)
                 {
                  X += 12;
                   if (X >= (XMAX-11))
                    {
                     X = 0;
                     Y += 32;
                    }
                  }
                 }   

              break;
            }

          default:
	    	{
              break;
            }

         }
}


void OLED_PutChar(unsigned char CHAR, unsigned char X, unsigned char Y, 
                  unsigned char color)
{
 unsigned char row, col;
 unsigned char a;
 unsigned int position;

  if (CHAR == 168) 
   { position = 6*(CHAR - 103); }
  else if (CHAR == 184) 
   { position = 6*(CHAR - 120); } 
  else if((CHAR<0x20)||(CHAR>0x7f))
   { position = 6*(CHAR - 192); }
  else
   { position = 6*(CHAR-0x20); }

             for(row=0;row<6;row++)
              for(col=0;col<8;col++)
               {
                if((CHAR<0x20)||(CHAR>0x7f))
                   a = pgm_read_byte(&cyrillic_font5x8[position+row]);
                else
                   a = pgm_read_byte(&Fonts5x8[position+row]);

                if ((a >> col) & 0x01)
                 LCD_PutPixel(X+row,Y+col, color);
               }
      
}



void OLED_PutChar_new(unsigned char CHAR, unsigned char X, unsigned char Y, 
                  unsigned char color)
{
 unsigned char row, col;
 unsigned char a;
 unsigned int position;

  if (CHAR == 168) 
   { position = 6*(CHAR - 103); }
  else if (CHAR == 184) 
   { position = 6*(CHAR - 120); } 
  else if((CHAR<0x20)||(CHAR>0x7f))
   { position = 6*(CHAR - 192); }
  else
   { position = 6*(CHAR-0x20); }

             for(row=0;row<6;row++)
			 {
                if((CHAR<0x20)||(CHAR>0x7f))
                   a = pgm_read_byte(&cyrillic_font5x8[position+5-row]);
                else
                   a = pgm_read_byte(&Fonts5x8[position+5-row]);
              for(col=0;col<8;col++)
               {

				if (color==1)
				{
                	if ((a >> col) & 0x01)
					{				 
                 		LCD_PutPixel(X+row,Y+7-col, 1);
					} else
					{
						LCD_PutPixel(X+row,Y+7-col, 0);
					}
				} 
				else
				{
                	if ( ((a >> col) & 0x01) )
					{				 
                 		LCD_PutPixel(X+row,Y+7-col, 0);
					} else
					{
                 		LCD_PutPixel(X+row,Y+7-col, 1);
					}
				};
               }
			  };
      
}

void OLED_PutChar_FONT2(unsigned char CHAR, unsigned char X, unsigned char Y, 
                        OLEDFontWeight font_weight, unsigned char color)
{
 unsigned char row, col;
 unsigned char a;
 unsigned int position;

  if (CHAR == 168) 
   { position = 6*(CHAR - 103); }
  else if (CHAR == 184) 
   { position = 6*(CHAR - 120); }
  else if((CHAR<0x20)||(CHAR>0x7f))
   { position = 6*(CHAR - 192); }
  else
   { position = 6*(CHAR-0x20); }

          for(row=0;row<(6*font_weight);row++)
              for(col=0;col<16;col++)
               {
                if((CHAR<0x20)||(CHAR>0x7f))
                   a = pgm_read_byte(&cyrillic_font5x8[position+(row/font_weight)]);
                else
                   a = pgm_read_byte(&Fonts5x8[position+(row/font_weight)]);

                if ((a >> (col/2)) & 0x01)
                  LCD_PutPixel(X+row,Y+col, color);
               }
      
}

void OLED_PutChar_FONT4(unsigned char CHAR, unsigned char X, unsigned char Y, 
                        OLEDFontWeight font_weight, unsigned char color)
{
 unsigned char row, col;
 unsigned char a;
 unsigned int position;

  
  if (CHAR == 168) 
   { position = 6*(CHAR - 103); }
  else if (CHAR == 184) 
   { position = 6*(CHAR - 120); } 
  else if((CHAR<0x20)||(CHAR>0x7f))
   { position = 6*(CHAR - 192); }
  else
   { position = 6*(CHAR-0x20); }

             for(row=0;row<(12*font_weight); row++)
              for(col=0;col<32;col++)
               {
                if((CHAR<0x20)||(CHAR>0x7f))
                   a = pgm_read_byte(&cyrillic_font5x8[position+(row/(font_weight*2))]);
                else
                   a = pgm_read_byte(&Fonts5x8[position+(row/(font_weight*2))]);

                if ((a >> (col/4)) & 0x01)
                  LCD_PutPixel(X+row,Y+col, color);
               }
      
}

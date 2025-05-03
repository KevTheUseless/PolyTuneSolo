#include "gpio.h"
#include "lcd.h"
#include "spi.h"
#include "lookup.h"

#include <stdio.h>

_lcd_dev lcd_dev;

uint16_t POINT_COLOR = BLACK, BACK_COLOR = WHITE;
uint16_t DeviceCode;

void LCD_WR_REG(uint8_t data)
{
  LCD_CS_CLR;
  LCD_RS_CLR;
  HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
  LCD_CS_SET;
}

void LCD_WR_DATA(uint8_t data)
{
  LCD_CS_CLR;
  LCD_RS_SET;
  HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
  LCD_CS_SET;
}

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD_WR_REG(LCD_Reg);
  LCD_WR_DATA(LCD_RegValue);
}

void LCD_WriteRAM_Prepare(void)
{
  LCD_WR_REG(lcd_dev.wramcmd);
}

void LCD_WriteData_16Bit(uint16_t data)
{
  LCD_CS_CLR;
  LCD_RS_SET;
  
  HAL_SPI_Transmit(&hspi2, (uint8_t*) &data + 1, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi2, (uint8_t*) &data, 1, HAL_MAX_DELAY);
  
  //HAL_SPI_Transmit(&hspi2, &data, 2, HAL_MAX_DELAY);
  LCD_CS_SET;
}

void LCD_SetWindows(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
  LCD_WR_REG(lcd_dev.setxcmd);
  LCD_WR_DATA(xStart >> 8);
  LCD_WR_DATA(0x00FF & xStart);
  LCD_WR_DATA(xEnd >> 8);
  LCD_WR_DATA(0x00FF & xEnd);

  LCD_WR_REG(lcd_dev.setycmd);
  LCD_WR_DATA(yStart >> 8);
  LCD_WR_DATA(0x00FF & yStart);
  LCD_WR_DATA(yEnd >> 8);
  LCD_WR_DATA(0x00FF & yEnd);

  LCD_WriteRAM_Prepare();
}

void LCD_SetCursor(uint16_t xPos, uint16_t yPos)
{
  LCD_SetWindows(xPos, yPos, xPos, yPos);
}

void LCD_DrawPoint(uint16_t x, uint16_t y)
{
  LCD_SetCursor(x, y);
  LCD_WriteData_16Bit(POINT_COLOR);
}

void LCD_Clear(uint16_t color)
{
  LCD_SetWindows(0, 0, lcd_dev.width - 1, lcd_dev.height - 1);
  LCD_CS_CLR;
  LCD_RS_SET;
  for (int i = 0; i < lcd_dev.height; i++)
  {
    for (int m = 0; m < lcd_dev.width; m++)
      LCD_WriteData_16Bit(color);
  }
  LCD_CS_SET;
}

void LCD_RESET(void)
{
  LCD_RST_CLR;
  HAL_Delay(100);
  LCD_RST_SET;
  HAL_Delay(100);
}

void LCD_Init(void)
{
  LCD_RESET();
  HAL_Delay(100);

  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD9);
  LCD_WR_DATA(0X30);
  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0X12);
  LCD_WR_DATA(0X81);
  LCD_WR_REG(0xE8);
  LCD_WR_DATA(0x85);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x7A);
  LCD_WR_REG(0xCB);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x02);
  LCD_WR_REG(0xF7);
  LCD_WR_DATA(0x20);
  LCD_WR_REG(0xEA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC0);
  LCD_WR_DATA(0x1B);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA(0x12);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x26);
  LCD_WR_REG(0xC7);
  LCD_WR_DATA(0XB7);
  LCD_WR_REG(0x36);
  LCD_WR_DATA(0x08);
  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);
  LCD_WR_REG(0xB1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x1A);
  LCD_WR_REG(0xB6);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0xA2);
  LCD_WR_REG(0xF2);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0x26);
  LCD_WR_DATA(0x01);
  LCD_WR_REG(0xE0);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x1D);
  LCD_WR_DATA(0x1A);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x0D);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x49);
  LCD_WR_DATA(0X66);
  LCD_WR_DATA(0x3B);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x11);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x09);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x04);
  LCD_WR_REG(0XE1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x18);
  LCD_WR_DATA(0x1D);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x13);
  LCD_WR_DATA(0x4C);
  LCD_WR_DATA(0x07);
  LCD_WR_DATA(0x13);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x2E);
  LCD_WR_DATA(0x2F);
  LCD_WR_DATA(0x05);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x3f);
  LCD_WR_REG(0x2A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xef);
  LCD_WR_REG(0x11);
  HAL_Delay(120);
  LCD_WR_REG(0x29);

  //LCD_LED = 1;
  LCD_Direction();
  LCD_Clear(WHITE);
}

void LCD_Direction()
{
  lcd_dev.setxcmd = 0x2A;
  lcd_dev.setycmd = 0x2B;
  lcd_dev.wramcmd = 0x2C;
  lcd_dev.width = LCD_W;
  lcd_dev.height = LCD_H;
  LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7));
}

void LCD_ShowChar(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t num, uint8_t size)
{  
  uint8_t temp;
	uint16_t temp_color = POINT_COLOR;      
		   
	num -= ' ';
	LCD_SetWindows(x, y, x + size / 2 - 1, y + size - 1);

  for (uint8_t pos = 0; pos < size; pos++)
  {
    temp = asc2_1608[num][pos];
    for (uint8_t t = 0; t < size / 2; t++)
    {                 
        if (temp & 0x01)
          LCD_WriteData_16Bit(fc); 
        else
          LCD_WriteData_16Bit(bc); 
        temp >>= 1; 
    
    }
  }
	POINT_COLOR = temp_color;	
	LCD_SetWindows(0, 0, lcd_dev.width - 1, lcd_dev.height - 1); 	 	  
}
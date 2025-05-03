#ifndef __LCD_H__
#define __LCD_H__

#include "stm32f1xx_hal.h"

typedef struct
{
  uint16_t width;
  uint16_t height;
  uint16_t id;
  uint8_t  dir;
  uint16_t wramcmd;
  uint16_t setxcmd;
  uint16_t setycmd;
} _lcd_dev;

#define LCD_W 240
#define LCD_H 320

#define LED     GPIO_PIN_9
#define LCD_CS  GPIO_PIN_11
#define LCD_RS  GPIO_PIN_10
#define LCD_RST GPIO_PIN_12


#define LCD_CS_SET  HAL_GPIO_WritePin(GPIOB, LCD_CS, GPIO_PIN_SET)
#define	LCD_RS_SET  HAL_GPIO_WritePin(GPIOB, LCD_RS, GPIO_PIN_SET)
#define	LCD_RST_SET	HAL_GPIO_WritePin(GPIOB, LCD_RST, GPIO_PIN_SET)

#define	LCD_CS_CLR  HAL_GPIO_WritePin(GPIOB, LCD_CS, GPIO_PIN_RESET)
#define	LCD_RS_CLR  HAL_GPIO_WritePin(GPIOB, LCD_RS, GPIO_PIN_RESET)
#define	LCD_RST_CLR HAL_GPIO_WritePin(GPIOB, LCD_RST, GPIO_PIN_RESET)

#define WHITE       0xFFFF
#define BLACK      	0x0000
#define BLUE       	0x001F
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40
#define GRAY  			0X8430

extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

void LCD_Init(void);
void LCD_Clear(uint16_t color);
void LCD_SetCursor(uint16_t xPos, uint16_t yPos);
void LCD_DrawPoint(uint16_t x, uint16_t y);
void LCD_SetWindows(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void LCD_WR_REG(uint8_t data);
void LCD_WR_DATA(uint8_t data);
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void Lcd_WriteData_16Bit(uint16_t data);
void LCD_SetWindows(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void LCD_Direction(void);

void LCD_ShowChar(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t num, uint8_t size);

#endif
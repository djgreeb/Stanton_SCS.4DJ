#include "spi_ili9341.h"
#include "font24.c"
#include "font20.c"
#include "font16.c"
#include "font12.c"
#include "font8.c"
//-------------------------------------------------------------------
extern SPI_HandleTypeDef hspi1;


//-------------------------------------------------------------------
uint16_t TFT9341_WIDTH;
uint16_t TFT9341_HEIGHT;
//-------------------------------------------------------------------

typedef struct
{
	uint16_t TextColor;
	uint16_t BackColor;
	sFONT *pFont;
}LCD_DrawPropTypeDef;
LCD_DrawPropTypeDef lcdprop;
//-------------------------------------------------------------------
void TFT9341_SendCommand(uint8_t cmd)
	{
  DC_COMMAND();
  SPI_Tmt(&hspi1, &cmd, 1, 3);
	}
//-------------------------------------------------------------------
void TFT9341_SendData(uint8_t dt)
	{	
	DC_DATA();
	SPI_Tmt(&hspi1, &dt, 1, 3);
	}
//-------------------------------------------------------------------
static void TFT9341_WriteData(uint8_t* buff, size_t buff_size) {
	DC_DATA();
	while(buff_size > 0) {
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		SPI_Tmt(&hspi1, buff, chunk_size, 100);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
}
//-------------------------------------------------------------------
void TFT9341_reset(void)
	{
	RESET_ACTIVE();
	HAL_Delay(150);					//delay in *100us
	RESET_IDLE();
	HAL_Delay(150);					//delay in *100us
	}
//-------------------------------------------------------------------
static void TFT9341_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  // column address set
  TFT9341_SendCommand(0x2A); // CASET
  {
    uint8_t data[4] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
    TFT9341_WriteData(data, 4);
  }
 
  // row address set
  TFT9341_SendCommand(0x2B); // RASET
  {
    uint8_t data[4] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
    TFT9341_WriteData(data, 4);
  }
 
  // write to RAM
  TFT9341_SendCommand(0x2C); // RAMWR
}//-------------------------------------------------------------------
void TFT9341_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
	{
	uint32_t YO;
	uint32_t i;	
  //if((x1 >= TFT9341_WIDTH) || (y1 >= TFT9341_HEIGHT) || (x2 >= TFT9341_WIDTH) || (y2 >= TFT9341_HEIGHT)) return;
	if(x1>x2) swap(x1,x2);
	if(y1>y2) swap(y1,y2);
  TFT9341_SetAddrWindow(x1, y1, x2, y2);
  uint8_t data[2] = { color >> 8, color & 0xFF };
	DC_DATA();
	YO = (x2-x1+1)*(y2-y1+1);
  for(i=0; i<YO; i++)
		{
    SPI_Tmt(&hspi1, data, 2, 10);
		}
	}
//-------------------------------------------------------------------
void TFT9341_FillScreen(uint16_t color)
{
  TFT9341_FillRect(0, 0, TFT9341_WIDTH-1, TFT9341_HEIGHT-1, color);
}
//-------------------------------------------------------------------
void TFT9341_DrawPixel(uint16_t x, uint8_t y, uint16_t color)
{	
  // column address set
  TFT9341_SendCommand(0x2A); // CASET
		{
    uint8_t data[2] = {x>>8, x&0xFF};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 2, 10);
		}
  // row address set
  TFT9341_SendCommand(0x2B); // RASET
		{
    uint8_t data[2] = {0, y};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 2, 10);
		}

	TFT9341_SendCommand(0x2C);
		{
		uint8_t clr[2] = {color>>8, color&0xFF};	
		DC_DATA();
		SPI_Tmt(&hspi1, clr, 2, 10);	
		}
}


//-------------------------------------------------------------------
void TFT9341_SendPixel(int x, int y, uint8_t colh, uint8_t coll)
	{
  TFT9341_SendCommand(0x2A); // CASET
		{
    uint8_t data[4] = {x>>8, x&0xFF, x>>8, x&0xFF};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 4, 10);
		}
  TFT9341_SendCommand(0x2B); // RASET
		{
    uint8_t data[4] = {y>>8, y&0xFF, y>>8, y&0xFF};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 4, 10);
		}

	TFT9341_SendCommand(0x2C);
		{
		uint8_t clr[2] = {colh, coll};	
		DC_DATA();
		SPI_Tmt(&hspi1, clr, 2, 10);
		}		
	}

//-------------------------------------------------------------------	
void TFT9341_BlackPixel(int x, int y)
	{
	 TFT9341_SendCommand(0x2A); // CASET
		{
    uint8_t data[4] = {x>>8, x&0xFF, x>>8, x&0xFF};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 4, 10);
		}
		
  TFT9341_SendCommand(0x2B); // RASET
		{
    uint8_t data[4] = {y>>8, y&0xFF, y>>8, y&0xFF};
		DC_DATA();
		SPI_Tmt(&hspi1, data, 4, 10);
		}

	TFT9341_SendCommand(0x2C);
		{
		uint8_t clr[2] = {0x00, 0x00};		
		DC_DATA();
		SPI_Tmt(&hspi1, clr, 2, 10);
		}		
	}
	
//------------------------------------------------------------------
void TFT9341_DrawLine(uint16_t color, uint16_t x1, uint16_t y1,
											uint16_t x2, uint16_t y2)
{
  int steep = abs(y2-y1)>abs(x2-x1);
  if(steep)
  {
    swap(x1,y1);
    swap(x2,y2);
  }
  if(x1>x2)
  {
    swap(x1,x2);
    swap(y1,y2);
  }
  int dx,dy;
  dx=x2-x1;
  dy=abs(y2-y1);
  int err=dx/2;
  int ystep;
  if(y1<y2) ystep=1;
  else ystep=-1;
  for(;x1<=x2;x1++)
  {
    if(steep) TFT9341_DrawPixel(y1,x1,color);
    else TFT9341_DrawPixel(x1,y1,color);
    err-=dy;
    if(err<0)
    {
      y1 += ystep;
      err+=dx;
    }
  }
}
//-------------------------------------------------------------------
void TFT9341_DrawRect(uint16_t color, uint16_t x1, uint16_t y1,
											uint16_t x2, uint16_t y2)
{
	TFT9341_DrawLine(color,x1,y1,x2,y1);
	TFT9341_DrawLine(color,x2,y1,x2,y2);
	TFT9341_DrawLine(color,x1,y1,x1,y2);
	TFT9341_DrawLine(color,x1,y2,x2,y2);
}
//-------------------------------------------------------------------
void TFT9341_DrawCircle(uint16_t x0, uint16_t y0, int r, uint16_t color)
{
	int f = 1-r;
	int ddF_x=1;
	int ddF_y=-2*r;
	int x = 0;
	int y = r;
	TFT9341_DrawPixel(x0,y0+r,color);
	TFT9341_DrawPixel(x0,y0-r,color);
	TFT9341_DrawPixel(x0+r,y0,color);
	TFT9341_DrawPixel(x0-r,y0,color);
	while (x<y)
	{
		if (f>=0)
		{
			y--;
			ddF_y+=2;
			f+=ddF_y;
		}
		x++;
		ddF_x+=2;
		f+=ddF_x;
		TFT9341_DrawPixel(x0+x,y0+y,color);
		TFT9341_DrawPixel(x0-x,y0+y,color);
		TFT9341_DrawPixel(x0+x,y0-y,color);
		TFT9341_DrawPixel(x0-x,y0-y,color);
		TFT9341_DrawPixel(x0+y,y0+x,color);
		TFT9341_DrawPixel(x0-y,y0+x,color);
		TFT9341_DrawPixel(x0+y,y0-x,color);
		TFT9341_DrawPixel(x0-y,y0-x,color);
	}
}
//-------------------------------------------------------------------
uint16_t TFT9341_RandColor(void)
{
	//return HAL_RNG_GetRandomNumber(&hrng)&0x0000FFFF;
}
//-------------------------------------------------------------------
void TFT9341_SetTextColor(uint16_t color)
{
  lcdprop.TextColor=color;
}
//------------------------------------
void TFT9341_SetBackColor(uint16_t color)
{
  lcdprop.BackColor=color;
}
//------------------------------------
void TFT9341_SetFont(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}
//-------------------------------------------------------------------
void TFT9341_DrawChar(uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width  = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];      
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
        TFT9341_DrawPixel((x + j), y, lcdprop.TextColor);
      }
      else
      {
        TFT9341_DrawPixel((x + j), y, lcdprop.BackColor);
      } 
    }
    y++;      
  }
}
//-------------------------------------------------------------------
void TFT9341_String(uint16_t x,uint16_t y, char *str)
{
  while(*str)
  {
    TFT9341_DrawChar(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}
//-------------------------------------------------------------------
void TFT9341_SetRotation(uint8_t r)
{
  TFT9341_SendCommand(0x36);
  switch(r)
  {
    case 0:
      TFT9341_SendData(0x48);
      TFT9341_WIDTH = 240;
      TFT9341_HEIGHT = 320;
      break;
    case 1:
      TFT9341_SendData(0x28);
      TFT9341_WIDTH = 320;
      TFT9341_HEIGHT = 240;
      break;
    case 2:
      TFT9341_SendData(0x88);
      TFT9341_WIDTH = 240;
      TFT9341_HEIGHT = 320;
      break;
    case 3:
      TFT9341_SendData(0xE8);
      TFT9341_WIDTH = 320;
      TFT9341_HEIGHT = 240;
      break;
  }
}
//-------------------------------------------------------------------
void TFT9341_FontsIni(void)
{
  Font8.Height = 8;
  Font8.Width = 5;
  Font12.Height = 12;
  Font12.Width = 7;
  Font16.Height = 16;
  Font16.Width = 11;
  Font20.Height = 20;
  Font20.Width = 14;
  Font24.Height = 24;
  Font24.Width = 17;
  lcdprop.BackColor=TFT9341_BLACK;
  lcdprop.TextColor=TFT9341_GREEN;
  lcdprop.pFont=&Font16;
}
//-------------------------------------------------------------------
void TFT9341_ini(uint16_t w_size, uint16_t h_size)
{
  uint8_t data[15];
  CS_ACTIVE();
  TFT9341_reset();
	HAL_Delay(1500);									//delay in *100us
	TFT9341_SendCommand(0x01);						//soft reset
	HAL_Delay(1500);						//delay in *100us
	TFT9341_SendCommand(0x11);						//sleep out
	HAL_Delay(100);						//delay in *100us
	TFT9341_SendCommand(0x3A);				//color mode set
	data[0] = 0x55;
	TFT9341_WriteData(data, 1);
	HAL_Delay(100);						//delay in *100us
	
	TFT9341_SendCommand(0x36);				//color mode set
	data[0] = 0xA0;			//0x60
	TFT9341_WriteData(data, 1);	
	HAL_Delay(10);					//delay in *100us
		
	TFT9341_SendCommand(0x2A);			//Column addr set, 4 args, no delay:		
	data[0] = 0x00;
	data[0] = 0x00;
	data[0] = 0x00;
	data[0] = 240;
	TFT9341_WriteData(data, 4);	
	HAL_Delay(10);				//delay in *100us
	
	TFT9341_SendCommand(0x2B);			//Row addr set, 4 args, no delay:	
	data[0] = 0x00;
	data[0] = 0x00;
	data[0] = 320>>8;
	data[0] = 320&0xFF;
	TFT9341_WriteData(data, 4);	
	HAL_Delay(10);					//delay in *100us
	
	TFT9341_SendCommand(0x21);					//inversion ON
	HAL_Delay(100);			//delay in *100us
	
	TFT9341_SendCommand(0x13);					//Normal display on
	HAL_Delay(100);			//delay in *100us

	TFT9341_SendCommand(0x29);

	
  TFT9341_WIDTH = 320;
  TFT9341_HEIGHT = 240;
  TFT9341_FontsIni();
	
}
//-------------------------------------------------------------------


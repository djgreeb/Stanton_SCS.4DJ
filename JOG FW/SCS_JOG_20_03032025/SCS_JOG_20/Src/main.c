/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2023 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "spi_ili9341.h"
#include "global_variables.h"
#include "gui_static.h"
#include "sector_giu.h"
#include "cue_giu.h"

extern __IO uint32_t uwTick;

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////////////
//
//		Product: Stanton JOG LCD
//
//		file global_variables.h - cooment this line, when deck B used
//
//
////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//	ver. 03032025
//	-	improved TFT9341_DrawPixel function. Delete extra 2 bytes removed CASET and RASET
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////FLASH MEM															 						 
//#define SETUP_START_ADDR   0x08040000																	 
//uint16_t WRITE_DATA = 0xFFFF;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
uint16_t CALC_COLOR(uint8_t intense, uint8_t color_num);
void RECALCULATE_SECTOR(void);
void DRAW_COVER(uint8_t num);
void DRAW_LOGO(void);
void BACKLIGHT_UP(void);
void BACKLIGHT_DOWN(void);
//void WriteFlash(uint32_t addr, uint16_t data);	

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();

  /* USER CODE BEGIN 2 */

	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOB, RST_Pin, GPIO_PIN_SET);
	CS_ACTIVE();
	__HAL_SPI_ENABLE(&hspi1);
	TIM2->CCR1 = 0;
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);		//PWM on
  TFT9341_ini(320, 240);
	DRAW_LOGO();		
	BACKLIGHT_UP();


	#ifdef DECK_A
	HAL_Delay(85000);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);		//turn on main lcd backlight 
	HAL_Delay(65000);
	#endif
	
	#ifndef DECK_A
	HAL_Delay(150000);
	#endif	
	
	BACKLIGHT_DOWN();
	TFT9341_FillScreen(TFT9341_BLACK);		
	for(px=0;px<76800;px++)
		{	
		_x = 319-(px%320);
		_y = px/320;
		A = _x-160;
		B = _y-120;
		A*=A;
		B*=B;	
		B+=A;	
		if(B<190)
			{
			//TFT9341_DrawPixel(_x, _y, TFT9341_BLACK);	
			}	
		else if(B<1130)
			{
			TFT9341_DrawPixel(_x, _y, 0x3188);	
			}	
		else if(B<7300)
			{
			TFT9341_DrawPixel(_x, _y, 0x4A4B);
			//WriteFlash(_x+(320*_y), 0x4A4B);	
			}
		else if(B<10001)
			{
			//TFT9341_DrawPixel(_x, _y, TFT9341_BLACK);	
			}	
		else
			{
			TFT9341_DrawPixel(_x, _y, 256*bkgnd[2*px+1]+bkgnd[2*px]);	
			//WriteFlash(_x+(320*_y), 256*bkgnd[2*px+1]+bkgnd[2*px]);	
			}
		//TFT9341_SendPixel(_x, _y, bkgnd[2*px+1], bkgnd[2*px]);	
		}			
		
	for(e=0;e<7260;e++)			//init variables
		{
		frame[e] = 0xFF;
		changes[e] = 0xFF;	
		}
	
	BACKLIGHT_UP();	
	HAL_Delay(10000);
	USART1->CR1 |= USART_CR1_RXNEIE; //interrupt for a RX Panel->STM enable
	USART2->CR1 |= USART_CR1_RXNEIE; //interrupt for a RX ARM->STM enable	
				
	HAL_TIM_Base_Start_IT(&htim5);	
	TIM5->CNT = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
	if(TOUCH_ENABLE==0)
		{		
		RECALCULATE_SECTOR();	
		}
	RedrawSector((sect_pos>>15)%135);	
				
	if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin)==0 && BUTTON_pressed==0)
		{
		BUTTON_pressed = 1;	
		}		
	else if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin)==1 && BUTTON_pressed==1)
		{
		BUTTON_pressed = 0;	
		}	
		
	
//	if(need_send_debug>0)
//		{
//		if(need_send_debug==1)
//			{
//			U_TX_DATA[0] = lcd_lay;
//			}
//		HAL_UART_Transmit(&huart1, U_TX_DATA, need_send_debug, 30);		
//		need_send_debug = 0;	
//		}		
		
	if(need_load_track>0)
		{
		if(need_load_track==6)
			{
			load_animation_t = uwTick; 	
			TIM2->CCR1 = 0;	
			first_loading = 0;	
			if(covr<3)
				{
				covr++;	
				}			
			else
				{
				covr = 0;	
				}			
			DRAW_COVER(covr);	
			PITCH = 0;	
			CUE_ADDR = uwTick%135; 
			HCUE_ADDR[0] = (CUE_ADDR+56)%135;	
			RedrawCue(CUE_ADDR, TFT9341_RED);	
			PREVIOUS_CUE_TYPE[0] = CUE_ADDR; 	
			PREVIOUS_CUE_TYPE[1] = 0; 	
			HCUE_ADDR[1]	= uwTick%135; 
			RedrawSector(CUE_ADDR);
			HCUE_ADDR[2]	= uwTick%135; 	
			sect_pos = CUE_ADDR<<15;	
			need_load_track--;	
			}			
		else if(need_load_track==5)
			{	
			if((uwTick-load_animation_t)>animation_wait)
				{
				TIM2->CCR1 = PWM_MAX;
				load_animation_t = uwTick;
				need_load_track--;	
				}		
			}
		else if(need_load_track==4)
			{			
			if((uwTick-load_animation_t)>animation_wait)
				{
				TIM2->CCR1 = 0;	
				load_animation_t = uwTick;
				need_load_track--;	
				}		
			}
		else if(need_load_track==3)
			{
			if((uwTick-load_animation_t)>animation_wait)
				{
				TIM2->CCR1 = PWM_MAX;
				load_animation_t = uwTick;
				need_load_track--;	
				}		
			}
		else if(need_load_track==2)
			{
			if((uwTick-load_animation_t)>animation_wait)
				{
				TIM2->CCR1 = 0;	
				load_animation_t = uwTick;
				need_load_track--;	
				}		
			}	
		else if(need_load_track==1)
			{
			if((uwTick-load_animation_t)>animation_wait)
				{
				CUE_LED_H;	
				TIM2->CCR1 = PWM_MAX;
				load_animation_t = uwTick;
				need_load_track--;	
				}			
			}			
		}		
		
	if(need_free_stop)
		{
		if((uwTick-stop_t)>74)
			{
			if(PITCH>5)
				{
				PITCH-=5;	
				}
			else
				{
				PITCH = 0;	
				need_free_stop = 0;	
				}		
			stop_t = uwTick;
			}		
			
		}		
		
	if(need_redraw_cue>0)
		{
		if((need_redraw_cue==4) && ((PREVIOUS_CUE_TYPE[0]!=CUE_ADDR) || (PREVIOUS_CUE_TYPE[1]!=0)))
			{
			RedrawCue(CUE_ADDR, TFT9341_RED);	
			PREVIOUS_CUE_TYPE[0] = CUE_ADDR; 	
			PREVIOUS_CUE_TYPE[1] = 0; 	
			}				
		else if((need_redraw_cue<4) && ((HCUE_ADDR[(need_redraw_cue-1)]!=PREVIOUS_CUE_TYPE[0]) || (PREVIOUS_CUE_TYPE[1]!=1)))	
			{
			RedrawCue(HCUE_ADDR[(need_redraw_cue-1)], TFT9341_GREEN);	
			PREVIOUS_CUE_TYPE[0] = HCUE_ADDR[(need_redraw_cue-1)]; 	
			PREVIOUS_CUE_TYPE[1] = 1; 	
			}	
		need_redraw_cue = 0;	
		}	
		
	if(need_turnoff==1)
		{
		CUE_LED_L;	
		PITCH = 0;
		RedrawCue((CUE_ADDR+5)%135, TFT9341_BLACK);
		BACKLIGHT_DOWN();
		DRAW_LOGO();	
		BACKLIGHT_UP();	
		while(1)
			{}			
		}

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/10000);					//1000

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */



	
///////////////////////////////////////////////////////////////////////////////
//
//
//	
//void WriteFlash(uint32_t addr, uint16_t data)
//	{
//		HAL_FLASH_Unlock();				
//		CLEAR_BIT(FLASH->CR, FLASH_CR_PG);		
//		if(FLASH_WaitForLastOperation(50000) != HAL_TIMEOUT)																												
//			{																																																					
//			__HAL_FLASH_CLEAR_FLAG(	FLASH_FLAG_EOP | FLASH_SR_PGPERR | FLASH_FLAG_BSY | FLASH_SR_PGSERR );																			
//			}			
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, SETUP_START_ADDR + 2*addr, data);							
//		HAL_FLASH_Lock();		
//	}	
	


///////////////////////////////////////////////////////////////////////////////
//
//   calculate color
//	
uint16_t CALC_COLOR(uint8_t intense, uint8_t color_num)
	{
	uint16_t r, g, b;	
	if(color_num==0)  //white
		{
		r = (31*intense)>>8;		
		g = (63*intense)>>8;	
		b = r;	
		}		
	else if(color_num==1)  //pioneer color
		{
		r = (24*intense)>>8;		
		g = (5*intense)>>8;	
		b = (7*intense)>>8;	
		}
	return r*2048 + g*32 + b;	
	}

	
///////////////////////////////////////////////////////////////////////////////
//
//   calculate sector
//		
void RECALCULATE_SECTOR(void)
	{	
	cnt_tick = TIM5->CNT;
	TIM5->CNT = 0;	
	cnt_tick*=PITCH;	
	sect_pos+= cnt_tick;
	sect_pos%= 4423680;
	}
	
	
	
///////////////////////////////////////////////////////////////////////////////
//
//   draw cover 0 - disc 1-from background 2-(cover 1) 3-(cover 2) 
//		
void DRAW_COVER(uint8_t num)
	{
	uint16_t i;	
	if(num==0)						//disc
		{
		for(i=0;i<29241;i++)
			{	
			_x = 75+(i%171);
			_y = 35+(i/171);
			A = _x-160;
			B = _y-120;
			A*=A;
			B*=B;	
			B+=A;	
			if(B<190)
				{
				TFT9341_DrawPixel(_x, _y, TFT9341_BLACK);	
				}	
			else if(B<1130)
				{
				TFT9341_DrawPixel(_x, _y, 0x3188);	
				}	
			else if(B<7300)
				{
				TFT9341_DrawPixel(_x, _y, 0x4A4B);	
				}	
			}		
		}		
	else if(num==1)								//background cover
		{
		for(i=0;i<29241;i++)
			{	
			_x = 75+(i%171);
			_y = 35+(i/171);
			A = _x-160;
			B = _y-120;
			A*=A;
			B*=B;	
			B+=A;	
			if(B<7300)
				{
				TFT9341_SendPixel(_x, _y, bkgnd[2*(_y*320+319-_x)+1], bkgnd[2*(_y*320+319-_x)]);
				}	
			}		
		}
	else if(num==2)								//cover 1
		{
		for(i=0;i<29241;i++)
			{	
			_x = 75+(i%171);
			_y = 35+(i/171);
			A = _x-160;
			B = _y-120;
			A*=A;
			B*=B;	
			B+=A;	
			if(B<7300)
				{
				TFT9341_SendPixel(_x, _y, cover_1[2*i+1], cover_1[2*i]);
				}	
			}	
		}				
	else if(num==3)								//cover 2
		{
		for(i=0;i<29241;i++)
			{	
			_x = 75+(i%171);
			_y = 35+(i/171);
			A = _x-160;
			B = _y-120;
			A*=A;
			B*=B;	
			B+=A;	
			if(B<7300)
				{
				TFT9341_SendPixel(_x, _y, cover_2[2*i+1], cover_2[2*i]);
				}	
			}		
		}		
	return;	
	}
	
	
#include "uart_panel.h"
#include "uart_arm.h"	
	
	
///////////////////////////////////////////////////////////////////////////////
//
// 
//	
void BACKLIGHT_UP(void)
	{
	for(e=0;e<PWM_MAX;e++)
		{
		TIM2->CCR1 = e;	
		HAL_Delay(40);			//Delay 4ms	
		}	
	TIM2->CCR1 = PWM_MAX;	
	}
	
///////////////////////////////////////////////////////////////////////////////
//
// 
//	
void BACKLIGHT_DOWN(void)
	{
	for(e=PWM_MAX;e>0;e--)
		{
		TIM2->CCR1 = e;	
		HAL_Delay(40);			//Delay 4ms
		}
	TIM2->CCR1 = 0;		
	}	

///////////////////////////////////////////////////////////////////////////////
//
// 
//		
void DRAW_LOGO(void)
	{
	TFT9341_FillScreen(TFT9341_BLACK);
	for(e=0;e<13720;e++)						/////// DRAW LOGO ON
		{
		if(LOGO[e]!=0x00)			//color not black
			{
			#ifdef DECK_A	
			TFT9341_DrawPixel(20+(e%280), 141-(e/280), CALC_COLOR(LOGO[e], 0));	
			#endif
				
			#ifndef DECK_A
			if((e%280)<48)
				{
				TFT9341_DrawPixel(20+(e%280), 141-(e/280), CALC_COLOR(LOGO[e], 0));	
				}
			else if((e%280)<166)
				{
				TFT9341_DrawPixel(20+(e%280), 141-(e/280), CALC_COLOR(LOGO[e], 1));	
				}		
			else if((e%280)<225)
				{
				TFT9341_DrawPixel(20+(e%280), 141-(e/280), CALC_COLOR(LOGO[e], 0));	
				}		
			else
				{
				TFT9341_DrawPixel(20+(e%280), 141-(e/280), CALC_COLOR(LOGO[e], 1));		
				}
			#endif	
			}	
		}	
	}	
	
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

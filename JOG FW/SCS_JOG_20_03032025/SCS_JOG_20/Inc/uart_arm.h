/////////////////////////////////////////////////////////////////////////////////////////////////
// 	uart handler ARM->STM
//	product: Stanton SCS.4DJ
//
//
//
//
//	Author: Anatska Andrei
//
/////////////////////////////////////////////////////////////////////////////////////////////////


	
///////////////////////////////////////////////////////////////////////////////
//
//		ARM->STM
//		
void USART2_IRQHandler(void)
	{
	LED_H;	
	if(USART2->SR & USART_SR_RXNE)		//have a new data
		{
		USART_RX_2 = (USART2->DR);
		arm_newtime = uwTick;
		if((arm_newtime - arm_oldtime)>arm_timeout)
			{	
			arm_data_len = 1;
			ARM_DATA[0] = USART_RX_2;
			}
		else if(arm_data_len<12)
			{
			ARM_DATA[arm_data_len] = USART_RX_2;
			arm_data_len++;	
			if(arm_data_len==3)				//data handler 0 (3 bytes)
				{			
				if(ARM_DATA[0]==0x90)			//leds in buttons
					{
					if(ARM_DATA[1]==PLAYL)			//Play led
						{							
						if(ARM_DATA[2]==0)
							{						
							if(need_free_stop==0)
								{
								PITCH = 0;
								}									
							play_led = 0;	
							}							
						else
							{
							PITCH = CALC_PITCH;		
							play_led = 1;		
							}							
						}
					else if(ARM_DATA[1]==VINYLL)			//VINYL led
						{
						if(ARM_DATA[2]==0)
							{
							vinylm_led = 0;	
							}							
						else
							{
							vinylm_led = 1;	
							}							
						}
					else if(ARM_DATA[1]==0x00)			//Turnoff proc
						{	
						if(ARM_DATA[2]==0x5A)
							{
							need_turnoff = 1;
							}	
						}	
						
					}
				}
			else if(arm_data_len==12)				//data handler 1 (12 bytes)
				{			
				if(ARM_DATA[0]==0x90 && ARM_DATA[3]==0x90 && ARM_DATA[6]==0x90 && ARM_DATA[9]==0x90)			//leds in buttons
					{
					if(ARM_DATA[1]==0x41 && ARM_DATA[4]==0x42 && ARM_DATA[7]==0x43 && ARM_DATA[10]==0x40)		//check
						{		
						if(ARM_DATA[2]==1)
							{
							lcd_lay = 2;
							}
						else if(ARM_DATA[5]==1)
							{
							lcd_lay = 3;	
							}					
						else if(ARM_DATA[8]==1)
							{
							lcd_lay = 4;		
							}	
						else if(ARM_DATA[11]==1)
							{
							lcd_lay = 1;		
							}	
						else
							{
							lcd_lay = 0;	
							}							
						}
					//need_send_debug = 1;	
					}
				}
			}
		arm_oldtime = arm_newtime; 			
		}
	LED_L;	
  HAL_UART_IRQHandler(&huart2);
	}		


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
																																									
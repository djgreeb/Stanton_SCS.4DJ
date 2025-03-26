/////////////////////////////////////////////////////////////////////////////////////////////////
// 	uart handler Panel->STM
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
//   Panel->STM
//		
void USART1_IRQHandler(void)
	{
	//LED_H;	
	if(USART1->SR & USART_SR_RXNE)		//have a new data
		{
		USART_RX_B = (USART1->DR);	
		panel_newtime = uwTick;
		if(((panel_newtime - panel_oldtime)>panel_timeout) || (panel_data_len==3 && (USART_RX_B==0x0B || USART_RX_B==0x90 || USART_RX_B==0x80)))
			{	
			panel_data_len = 1;
			PANEL_DATA[0] = USART_RX_B;
			}
		else if(panel_data_len<3)
			{
			PANEL_DATA[panel_data_len] = USART_RX_B;
			panel_data_len++;	
			if(panel_data_len==3)
				{			
				if(PANEL_DATA[0]==0xB0)					//JOG rotate and 1/2 pitch
					{
					if(PANEL_DATA[1]==JOGVINYL)	
						{
						if(vinylm_led==1 && first_loading==0)
							{						
							if(PANEL_DATA[2]>0x3F)
								{
								tmp_rotate = (PANEL_DATA[2]-0x40)*890; 	
								sect_pos+= tmp_rotate;
								sect_pos%= 4423680;	
								}
							else
								{
								tmp_rotate = (0x40-PANEL_DATA[2])*890;	
								tmp_sect_pos = tmp_rotate;	
								tmp_sect_pos%= 4423680;	
								sect_pos+= 4423680-tmp_sect_pos; 
								sect_pos%= 4423680;				
								}
							TOUCH_ENABLE = 1;
							PITCH = 0;
							}								
						}
					else if(PANEL_DATA[1]==JOGCDJ)	
						{
						if(TOUCH_ENABLE==1 && vinylm_led==1 && first_loading==0)	
							{
							if(PANEL_DATA[2]>0x3F)
								{
								tmp_rotate = (PANEL_DATA[2]-0x40)*890;	
								tmp_sect_pos = tmp_rotate;
								if(tmp_sect_pos>890)
									{
									sect_pos+= tmp_sect_pos;
									sect_pos%= 4423680;	
									}
								else
									{
									TOUCH_ENABLE = 0;
									if(play_led)
										{
										PITCH = CALC_PITCH;	
										}										
									TIM5->CNT = 0;	
									}									
								}
							else
								{
								tmp_rotate = (0x40-PANEL_DATA[2])*890;	
								tmp_sect_pos = tmp_rotate;	
								tmp_sect_pos%= 4423680;	
								if(tmp_sect_pos>890)
									{	
									sect_pos+= 4423680-tmp_sect_pos; 
									sect_pos%= 4423680;
									}
								else
									{
									TOUCH_ENABLE = 0;		
									if(play_led)
										{
										PITCH = CALC_PITCH;	
										}	
									TIM5->CNT = 0;	
									}			
								}								
							}
						}
					else if(PANEL_DATA[1]==PTCH_FDR)		//1/2 pitch		
						{
						CALC_PITCH = 1000 + ((127-PANEL_DATA[2])<<1); 
						if(play_led)
							{
							PITCH = CALC_PITCH;	
							}						
						}							
					}
				else if(PANEL_DATA[0]==0xB1)					//2/2 pitch
					{
					if(PANEL_DATA[1]==PTCH_FDR)	
						{	
						CALC_PITCH = 1000 - (PANEL_DATA[2]<<1); 	
						if(play_led)
							{
							PITCH = CALC_PITCH;	
							}	
						}
					}						
				else if(PANEL_DATA[0]==0x90 && PANEL_DATA[2]==0x01)					//button press
					{
					if(PANEL_DATA[1]==SENSOR)		//sensor press
						{
						if(vinylm_led)
							{		
							if(first_loading==0)
								{	
								TOUCH_ENABLE = 1;	
								PITCH = 0;
								}	
							}								
						}	
					else if(PANEL_DATA[1]==CUE)		//CUE press
						{
						if(first_loading==0)
							{
							if(play_led==0 && ((sect_pos>>15)!=CUE_ADDR) && TOUCH_ENABLE)
								{
								CUE_ADDR = sect_pos>>15;	
								need_redraw_cue = 4;	
								}								
							else
								{
								sect_pos = CUE_ADDR<<15;
								need_redraw_cue = 4;
								}									
							CUE_B_pressed = 1;	
							}	
						}	
					else if(PANEL_DATA[1]==PLAY)		//PLAY press
						{
						if(play_led==1)
							{							
							if(CUE_B_pressed)
								{
								continue_playback = 1;	
								}
							else if(TOUCH_ENABLE==0)		//free stop
								{
								need_free_stop = 1;	
								stop_t = uwTick;
								}								
							}
						}				
					else if(PANEL_DATA[1]==0x02)		//Display button L
						{						
						if(lcd_lay==1 && HCUE_lay==0)
							{
							HCUE_lay = 1;
							}
						else if(lcd_lay==1 && HCUE_lay==1)
							{
							HCUE_lay = 0;
							}
						}
					else if(PANEL_DATA[1]==0x01)		//Display button R
						{	
						if(lcd_lay==1 && HCUE_lay==1)
							{
							if(HCUE_DEL_lay==0)
								{
								HCUE_DEL_lay = 1;	
								}
							else
								{
								HCUE_DEL_lay = 0;	
								}
							}
						}
					else if(PANEL_DATA[1]==BTN_1)		//Display button 1
						{
						if(lcd_lay==1 && HCUE_lay==1)			//HOT CUE 1
							{
							if(first_loading==0)
								{
								if(HCUE_DEL_lay==0)
									{
									sect_pos = HCUE_ADDR[0]<<15;
									PITCH = CALC_PITCH;		
									need_redraw_cue = 1;	
									}
								else
									{
									HCUE_DEL_lay = 0;	
									}										
								}
							}		
						}	
					else if(PANEL_DATA[1]==BTN_2)		//Display button 2
						{
						if(lcd_lay==1 && HCUE_lay==1)			//HOT CUE 2
							{
							if(first_loading==0)
								{
								if(HCUE_DEL_lay==0)
									{
									sect_pos = HCUE_ADDR[1]<<15;
									PITCH = CALC_PITCH;		
									need_redraw_cue = 2;	
									}
								else
									{
									HCUE_DEL_lay = 0;	
									}	
								}									
							}	
						}	
					else if(PANEL_DATA[1]==BTN_3)		//Display button 3
						{
						if(lcd_lay==1 && HCUE_lay==1)			//HOT CUE 3
							{
							if(first_loading==0)
								{	
								if(HCUE_DEL_lay==0)
									{
									sect_pos = HCUE_ADDR[2]<<15;
									PITCH = CALC_PITCH;		
									need_redraw_cue = 3;	
									}
								else
									{
									HCUE_DEL_lay = 0;	
									}									
								}
							}							
						else if(lcd_lay==2 || lcd_lay==3)
							{							
							need_load_track = 6;
							}
						}			
					}
				else if(PANEL_DATA[0]==0x80 && PANEL_DATA[2]==0x00)					//button unpress
					{
					if(PANEL_DATA[1]==SENSOR)		//sensor unpress
						{
						if(vinylm_led)
							{							
							if(tmp_rotate<=890)
								{								
								TOUCH_ENABLE = 0;		
								if(play_led)
									{
									PITCH = CALC_PITCH;	
									}		
								TIM5->CNT = 0;
								}							
							}								
						}		
					else if(PANEL_DATA[1]==CUE)		//CUE unpress
						{
						if(first_loading==0)
							{		
							if(continue_playback)
								{
								continue_playback = 0;	
								}
							else
								{
								sect_pos = CUE_ADDR<<15;
								PITCH = 0;		
								}								
							CUE_B_pressed = 0;	
							}								
						}	
					}				
				}		
			}	
		panel_oldtime = panel_newtime; 			
		}
	//LED_L;	
  HAL_UART_IRQHandler(&huart1);
	}	



//////////////////////////////////////////////////////////////////////////////////////////////////
				
																																									
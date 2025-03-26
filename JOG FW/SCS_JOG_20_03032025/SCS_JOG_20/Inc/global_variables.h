/////////////////////////////////////////////////////////////////////////////////////////////////
// 	global variables
//	product: Stanton SCS.4DJ
//
//
//
//
//	Author: Anatska Andrei
//
/////////////////////////////////////////////////////////////////////////////////////////////////


///////DECK CONFIGURATION//////

#define DECK_A							//cooment this line, when deck B used



/* DECKS variables ---------------------------------------------------------*/
uint8_t lcd_lay = 0;			//display layer 0 - unknown, 1 - waveform, 2 - browse, 3 - playlist, 4 - settings
uint8_t play_led = 0;
uint8_t vinylm_led = 0;
uint8_t HCUE_lay = 0;
uint8_t HCUE_DEL_lay = 0;
uint8_t CUE_B_pressed = 0;
uint8_t continue_playback = 0;
uint8_t need_free_stop = 0;


#ifdef DECK_A

#endif

#ifndef DECK_A


#endif	
										



/* Display variables ---------------------------------------------------------*/
extern uint16_t TFT9341_WIDTH;
extern uint16_t TFT9341_HEIGHT;

uint8_t G = 0;
uint8_t prev_sec = 0;	
uint8_t prev_cue = 0;		
uint8_t PREVIOUS_CUE_TYPE[2] = {0,0};		//cue pos and type 0 - red, 1 - green		
int16_t A, B;	
uint16_t d_picture = 0;
uint16_t e;	
#define ABS(X)  ((X) > 0 ? (X) : -(X))  		
#define w_cue 4	

uint16_t _x, _y;	
	
uint8_t frame[7260];
uint8_t changes[7260];
uint32_t px;
uint8_t mask;
#define PWM_MAX 170

uint8_t first_loading = 1;
uint8_t need_load_track = 0;
uint8_t need_turnoff = 0;
uint8_t need_redraw_cue = 0;
uint32_t load_animation_t;
uint32_t stop_t;
#define animation_wait 			1400

/* JOG rotate variables ---------------------------------------------------------*/
uint32_t cnt_tick, sect_pos, tmp_sect_pos, tmp_rotate;
uint16_t PITCH = 0;			//100.0%
uint16_t CALC_PITCH = 1000;	
uint8_t TOUCH_ENABLE = 0;
uint8_t CUE_ADDR = 0;
uint8_t HCUE_ADDR[3] = {0,0,0};


/* UART Panel->STM variables ---------------------------------------------------------*/
#define panel_timeout		1
uint32_t panel_oldtime = 0;
uint32_t panel_newtime;
uint8_t USART_RX_B;
uint8_t PANEL_DATA[3];	
uint8_t panel_data_len = 0;
uint8_t U_TX_DATA[25];
#ifdef DECK_A
#define SENSOR 		0x2C
#define CUE 			0x26
#define PLAY 			0x24
#define JOGCDJ 		0x0C
#define JOGVINYL 	0x0E
#define BTN_0			0x02
#define BTN_1			0x04
#define BTN_2			0x06
#define BTN_3			0x08
#define PTCH_FDR	0x00

#endif


#ifndef DECK_A
#define SENSOR 		0x2B
#define CUE 			0x25
#define PLAY 			0x23
#define JOGCDJ 		0x0D
#define JOGVINYL 	0x0F
#define BTN_0			0x01
#define BTN_1			0x03
#define BTN_2			0x05
#define BTN_3			0x07
#define PTCH_FDR	0x01

#endif	



/* UART ARM->STM variables ---------------------------------------------------------*/
#define arm_timeout		1
uint32_t arm_oldtime = 0;
uint32_t arm_newtime;
uint8_t USART_RX_2;	
uint8_t ARM_DATA[12];	
uint8_t arm_data_len = 0;


#ifdef DECK_A
#define PLAYL 		0x24
#define VINYLL		0x22	


#endif


#ifndef DECK_A
#define PLAYL 		0x23
#define VINYLL		0x21	



#endif	

#define WAVEL 		0x40
#define BRWSL 		0x41
#define PLSTL 		0x42
#define STNGS 		0x43



////temporary
uint8_t need_send_debug = 0;


/* BUTTON variables ---------------------------------------------------------*/
uint8_t BUTTON_pressed = 0;
uint8_t covr = 0;
#define LED_H			GPIOC->BSRR = 0x00002000	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define LED_L			GPIOC->BSRR = 0x20000000	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define CUE_LED_L			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET)
#define CUE_LED_H			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET)



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
																																									
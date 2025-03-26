/////////////////////////////////////////////////////////////////////////////////////////////////
// 	sector giu
//	product: Stanton SCS.4DJ
//
//
//
//
//	Author: Anatska Andrei
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void RedrawSector(uint8_t n);					


///////////////////////////////////////////////////////////////////////////////
//
//
//
void RedrawSector(uint8_t n)
	{
	if(prev_sec==n)
		{
		return;	
		}	

	uint16_t sd8;
	uint8_t so8;
	uint32_t px;	
	uint16_t s;
	uint16_t i, limit;	
	
	if(n<135)
		{	
		limit = DOT_MAP[prev_sec][0];
		for(i=0;i<limit;i++)												//black to white
			{
			s = DOT_MAP[prev_sec][1 + i];	
			changes[s>>3]|= 1<<(s%8);						
			}
			
		limit = DOT_MAP[n][0];	
		for(i=0;i<limit;i++)														//white to black
			{
			s = DOT_MAP[n][1 + i];	
			changes[s>>3]&= ~(1<<(s%8));		
			}	
			
		for(s=40;s<58040;s++)
			{		
			sd8 = s>>3;
			so8 = s%8;		
			if(((frame[sd8]>>so8)&0x01)==0)			//black
				{
				if(((changes[sd8]>>so8)&0x01)!=0)
					{
					frame[sd8]|= 1<<so8;			
					//TFT9341_DrawPixel(39+(s%242), s/242, 0xFFFF);	
					px = 2*(320*(s/242) + 280 - (s%242));
					//TFT9341_SendPixel(39+(s%242), s/242, bkgnd[px+1], bkgnd[px]);	
					TFT9341_DrawPixel(39+(s%242), s/242, 256*bkgnd[px+1]+bkgnd[px]);			
					}					
				}
			else
				{
				if(((changes[sd8]>>so8)&0x01)==0)
					{												
					frame[sd8]&= ~(1<<so8);	
					//TFT9341_DrawPixel(39+(s%242), s/242, 0x0000);		
					TFT9341_BlackPixel(39+(s%242), s/242);		
					}		
				}	
			}		
		prev_sec = n;		
		}	
	return;
	};

	


	
	


	
	


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
																																									
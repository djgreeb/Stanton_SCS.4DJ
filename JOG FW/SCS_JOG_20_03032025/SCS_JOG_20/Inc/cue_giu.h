/////////////////////////////////////////////////////////////////////////////////////////////////
// 	cue giu
//	product: Stanton SCS.4DJ
//
//
//
//
//	Author: Anatska Andrei
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void RedrawCue(uint8_t n, uint16_t color);					
void CueTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
void CueLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);	



///////////////////////////////////////////////////////////////////////////////
//
//
//
void RedrawCue(uint8_t n, uint16_t color)
	{
	n = (n+71)%135;			//add offset
	if(prev_cue==n)
		{
		return;	
		}	
	uint8_t C;			
	if(n<135)
		{	
		d_picture = TFT9341_BLACK;
		C = prev_cue;
		if((12<C && C<18) || (45<C && C<51) || (79<C && C<84) || (113<C && C<119))
			{
			CueTriangle(40+cord[C][0], 40+cord[(C+w_cue)%135][0], 160, cord[C][1], cord[(C+w_cue)%135][1], 120);	
			}	
		else if((12<C && C<30) || (45<C && C<64) || (79<C && C<98) || (113<C && C<131))
			{
			CueTriangle(40+cord[C][0], 160, 40+cord[(C+w_cue)%135][0], cord[C][1], 120, cord[(C+w_cue)%135][1]);						// - A	
			}				
		else
			{		
			CueTriangle(160, 40+cord[(C+w_cue)%135][0], 40+cord[C][0], 120, cord[(C+w_cue)%135][1], cord[C][1]);				//B	
			}
		d_picture = color;	
		C = n;		
		if((12<C && C<18) || (45<C && C<51) || (79<C && C<84) || (113<C && C<119))
			{
			CueTriangle(40+cord[C][0], 40+cord[(C+w_cue)%135][0], 160, cord[C][1], cord[(C+w_cue)%135][1], 120);	
			}	
		else if((12<C && C<30) || (45<C && C<64) || (79<C && C<98) || (113<C && C<131))
			{
			CueTriangle(40+cord[C][0], 160, 40+cord[(C+w_cue)%135][0], cord[C][1], 120, cord[(C+w_cue)%135][1]);						// - A	
			}				
		else
			{		
			CueTriangle(160, 40+cord[(C+w_cue)%135][0], 40+cord[C][0], 120, cord[(C+w_cue)%135][1], cord[C][1]);				//B	
			}	
		prev_cue = n;
		}	
	return;
	};	

	
	
///////////////////////////////////////////////////////////////////////////////
//
//
//
void CueTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
	{   
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
		{
    xinc1 = 1;
    xinc2 = 1;
		}
  else                          /* The x-values are decreasing */
		{
    xinc1 = -1;
    xinc2 = -1;
		}
  
  if (y2 >= y1)                 /* The y-values are increasing */
		{
    yinc1 = 1;
    yinc2 = 1;
		}
  else                          /* The y-values are decreasing */
		{
    yinc1 = -1;
    yinc2 = -1;
		}
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
		{
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* There are more x-values than y-values */
		}
  else                          /* There is at least one y-value for every x-value */
		{
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* There are more y-values than x-values */
		}
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
		{
    CueLine(x, y, x3, y3);
    
    num += num_add;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
			{
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
			}
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
		} 
	}

///////////////////////////////////////////////////////////////////////////////
//
//
//
void CueLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
	{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
		{
    xinc1 = 1;
    xinc2 = 1;
		}
  else                          /* The x-values are decreasing */
		{
    xinc1 = -1;
    xinc2 = -1;
		}
  
  if (y2 >= y1)                 /* The y-values are increasing */
		{
    yinc1 = 1;
    yinc2 = 1;
		}
  else                          /* The y-values are decreasing */
		{
    yinc1 = -1;
    yinc2 = -1;
		}
  
  if(deltax >= deltay)         /* There is at least one x-value for every y-value */
		{
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* There are more x-values than y-values */
		}
  else                          /* There is at least one y-value for every x-value */
		{
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* There are more y-values than x-values */
		}
		
  for(curpixel = 0; curpixel <= num_pixels; curpixel++)
		{		
		A = x-160;
		B = y-120;
		A*=A;
		B*=B;	
		B+=A;	
		if(B>7600 && B<9500)	
			{
			TFT9341_DrawPixel(x, y, d_picture);
			//WriteFlash(x+(320*y), TFT9341_RED);		
			}		
    num += num_add;                            /* Increase the numerator by the top of the fraction */
    if(num >= den)                           /* Check if numerator >= denominator */
			{
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
			}
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
		}
	return;		
	}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
																																									
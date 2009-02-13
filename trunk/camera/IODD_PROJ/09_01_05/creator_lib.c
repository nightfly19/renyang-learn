// --------------------------------------------------------------------
//
//   Title     :  Creator_lib
//             :
//   Library   :
//             :
//   Developers:  MICROTIME MDS group
//             :
//   Purpose   :  DEMO Program for Creator Board
//             :
//   Limitation:
//             :
//   Note      :
//             :
// --------------------------------------------------------------------
//   modification history :
// --------------------------------------------------------------------
//   Version| mod. date: |
//   V1.0   | 03/02/2004 | First release
// --------------------------------------------------------------------
//
// Note:
//
//       MICROTIME COMPUTER INC.
//
//
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>

#include "def.h"
#include "creator_pxa270_lcd.h"
#include "creator_lib.h"



int 
Delay(int ms)
{
	usleep(ms *1000);	
}

	
/* =======================================================================
 7 Segment LED functions
  ===================================================================== */
int
_7SEG_Starting(int fd, unsigned char byIsStarting)
{
	    int 		ret;

        if (byIsStarting == TRUE)
            ret = ioctl(fd, _7SEG_IOCTL_ON, NULL);
        else    
    	    ret = ioctl(fd, _7SEG_IOCTL_OFF, NULL);
        return ((ret >= 0) ? OK : UM);    	    
}
    
  
int
_7SEG_put_led(int fd, unsigned which, unsigned long data)
{
	_7seg_info_t	Segment;	
	int 		ret;

	Segment.Mode = _7SEG_MODE_HEX_VALUE;
	Segment.Which = which;
	Segment.Value = data;
	ret = ioctl(fd, _7SEG_IOCTL_SET, &Segment);
    	
	return ((ret >= 0) ? OK : UM);    	
}



int
_7SEG_Black(int fd)
{
	_7seg_info_t	Segment;	
	int 			ret;

	Segment.Mode = _7SEG_MODE_PATTERN;
	Segment.Which = (~0);
	Segment.Value = 0;
	ret = ioctl(fd, _7SEG_IOCTL_SET, &Segment);
    	
	return ((ret >= 0) ? OK : UM);    	
}


/* =======================================================================
 LED Lamp functions
  ===================================================================== */
int 
HUHU_put_led(int fd, unsigned char value)
{
	int 		ret;
	unsigned short	data = value;	
	
	ret = ioctl(fd, LED_IOCTL_SET, &data);
	return ((ret >= 0) ? OK : UM);	
}



/* =======================================================================
 DIP Switch functions
  ===================================================================== */
int 
DIPSW_get_sw (int fd, unsigned char *mask)
{
	unsigned short	data;
	int		ret;
	
	ret = ioctl(fd, DIPSW_IOCTL_GET, &data);	
	
	*mask = (unsigned char)data;
	
	return ((ret >= 0) ? OK : UM);
}



/* =======================================================================
 KEYPAD  functions
  ===================================================================== */
int 
KEYPAD_clear_buffer(int fd)
{
	unsigned short key;
				
	/* 清除之前輸入的殘留資料	*/
	return ((ioctl(fd, KEY_IOCTL_CLEAR, &key) >= 0) ? OK : UM); 
}

  
  
int 
KEYPAD_get_key (int fd, unsigned short *key)
{
	int	ret ;
	
	ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, key);
	if (ret < 0)
	    return (UM);	
	    
	ret = ioctl(fd, KEY_IOCTL_GET_CHAR, key);
	if (ret < 0)
	    return (UM);
	    	
	*key = *key >> 8;
	
	return (OK);
}



int 
KEYPAD_wait_key (int fd, unsigned short *key)
{
	int	ret ;
	
	ret = ioctl(fd, KEY_IOCTL_WAIT_CHAR, key);
	if (ret < 0)
	    return (UM);	

	*key = *key >> 8;
	
	return (OK);
}



/*************************************************************************
Wait key 
*************************************************************************/
void 
WaitChar(int fd)
{
	unsigned short 	key;
	
	while(KEYPAD_wait_key(fd, &key) != OK);
}



/*************************************************************************
Get Key 
*************************************************************************/
unsigned char 
GetIntNumRange (int fd, unsigned char MinNum, unsigned char MaxNum)
{
	unsigned short key;
	unsigned char ch;
		
	if (KEYPAD_wait_key(fd, &key) == OK){
	    ch = key;	
	    if (ch >= MinNum && ch <= MaxNum) 
	        return(ch);
	}
	return (0xff);
}



/*************************************************************************
 LCD function 
*************************************************************************/
int 
LCD_fClearScreen(int fd)
{
	/* 清除LCD 上的資料 */
	return ((ioctl(fd, LCD_IOCTL_CLEAR, NULL) >= 0) ? OK : UM);	
}
	
	

int 
LCD_put_string(int fd, char *string)
{
	lcd_write_info_t	display;
	int			ret;
	
	display.Count = sprintf(display.Msg, "%s", string);
	ret = ioctl(fd, LCD_IOCTL_WRITE, &display);
	
	return ((ret >= 0) ? OK : UM);
}



int
LCD_fprintf (int fd, char *fmt,...)
{
        va_list     ap;
        char        string[512];

        va_start(ap, fmt);
        vsprintf(string, fmt, ap);
        va_end(ap);
        
        return (LCD_put_string(fd, string));
}


void
LCD_ErrorMessage(int fd_lcd, char *msg)
{
    	LCD_EnableCursor(fd_lcd, TRUE);	    
        LCD_fprintf(fd_lcd, "%s", msg);	
        WaitChar(fd_lcd);	
}



int
LCD_EnableCursor(int fd, int IsCursorOn)
{
        int	ret, command ;
	
        command = (IsCursorOn != FALSE) ? LCD_IOCTL_CUR_ON : LCD_IOCTL_CUR_OFF ;
        ret = ioctl(fd, command, NULL);	
	
        return ((ret >= 0) ? OK : UM);	
}	



int
LCD_Cursor (int fd, int x, int y)
{
	    lcd_write_info_t	display;    
        int	                ret ;
        
	
	    display.CursorX = x; 
	    display.CursorY = y;
	    ret = ioctl(fd, LCD_IOCTL_CUR_SET, &display);
	
        return ((ret >= 0) ? OK : UM);		    
}    



static int gLCD_fd = -1;
void
LCD_SetHandle (int fd)
{
       gLCD_fd = fd;    
}    


int
LCD_printf (char *fmt,...)
{
        va_list     ap;
        char        string[512];

        va_start(ap, fmt);
        vsprintf(string, fmt, ap);
        va_end(ap);
        
        return (LCD_put_string(gLCD_fd, string));
}


int
LCD_ClearScreen(void)
{
        return (LCD_fClearScreen(gLCD_fd));    
}    

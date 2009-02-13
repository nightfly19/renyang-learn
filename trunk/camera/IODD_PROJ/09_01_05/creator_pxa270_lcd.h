//=============================================================================
// File Name : creator_s3c4510_lcd.h
// Function  : LCD device drvier define
// Program   :
// Date      : 03/05/2004
// Version   : 1.00
// History
//   1.0.0 : Programming start (03/05/2004) -> SOP
//=============================================================================
 
#ifndef _CREATOR_S3C2410_LCD_H_ 
#define _CREATOR_S3C2410_LCD_H_ 

//#include <linux/config.h>
#if defined(__linux__)
#include <asm/ioctl.h>		/* For _IO* macros */
#define LCD_IOCTL_NR(n)	     		_IOC_NR(n)
#elif defined(__FreeBSD__)
#include <sys/ioccom.h>
#define LCD_IOCTL_NR(n)	     		((n) & 0xff)
#endif

#define LCD_MAJOR_NUM			120
#define LCD_IOCTL_MAGIC			LCD_MAJOR_NUM
#define LCD_IO(nr)			_IO(LCD_IOCTL_MAGIC,nr)
#define LCD_IOR(nr,size)		_IOR(LCD_IOCTL_MAGIC,nr,size)
#define LCD_IOW(nr,size)		_IOW(LCD_IOCTL_MAGIC,nr,size)
#define LCD_IOWR(nr,size)		_IOWR(LCD_IOCTL_MAGIC,nr,size)

// function headers

typedef struct lcd_write_info {		/* for LCD 			*/
	unsigned char  Msg[512] ;	/* 輸入的字元存放區，最大512    */
	unsigned short Count;		/* 實際輸入的字元長度 		*/	
	int            CursorX, CursorY;/* 讀取 cursor 設定游標之X, Y	*/
} lcd_write_info_t ;

typedef struct lcd_full_image_info {
	unsigned short data[0x800];	/* LCD整個資料 */
} lcd_full_image_info_t;

// 游標各種操作模式
#define WHITE_BLINK_CURSOR		0	// 白色閃爍游標
#define BLACK_BLINK_CURSOR		1	// 黑色閃爍游標
#define REVERSE_CURSOR			2	// 黑白交換的游標
#define REVERSE_BLINK_CURSOR		3	// 黑白閃爍交換的游標

#define TABS				4	// 多少空白字代表TAB

/* Key Pad scan code define */
#define VK_S2				1	/* ASCII = '1'		*/	
#define VK_S3				2	/* ASCII = '2'		*/
#define VK_S4				3	/* ASCII = '3'		*/
#define VK_S5				10	/* ASCII = 'A'		*/
#define VK_S6				4	/* ASCII = '4'		*/
#define VK_S7				5	/* ASCII = '5'		*/
#define VK_S8				6	/* ASCII = '6'		*/
#define VK_S9				11	/* ASCII = 'B'		*/
#define VK_S10				7	/* ASCII = '7'		*/
#define VK_S11				8	/* ASCII = '8'		*/
#define VK_S12				9	/* ASCII = '9'		*/
#define VK_S13				12	/* ASCII = 'C'		*/
#define VK_S14				14	/* ASCII = '*'		*/
#define VK_S15				0	/* ASCII = '0'		*/
#define VK_S16				15	/* ASCII = '#'		*/
#define VK_S17				13	/* ASCII = 'D'		*/

enum {
  VK_CHAR_0=0,	VK_CHAR_1,	VK_CHAR_2,	VK_CHAR_3,		
  VK_CHAR_4,	VK_CHAR_5,	VK_CHAR_6,	VK_CHAR_7,	
  VK_CHAR_8,	VK_CHAR_9,	VK_CHAR_A,	VK_CHAR_B,	  
  VK_CHAR_C,	VK_CHAR_D,	VK_CHAR_STAR,	VK_CHAR_POND  
} ;	


/* LED define */
#define LED_ALL_ON			0xFF	/* 點亮LED Lamp		*/
#define LED_ALL_OFF			0x00	/* 熄滅LED Lamp		*/
#define LED_D9_INDEX			0	/* LED 編號D9 (1)	*/
#define LED_D10_INDEX			1	/* LED 編號D10(2)	*/
#define LED_D11_INDEX			2	/* LED 編號D11(3)	*/
#define LED_D12_INDEX			3	/* LED 編號D12(4)	*/
#define LED_D13_INDEX			4	/* LED 編號D13(5)	*/
#define LED_D14_INDEX			5	/* LED 編號D14(6)	*/
#define LED_D15_INDEX			6	/* LED 編號D15(7)	*/
#define LED_D16_INDEX			7	/* LED 編號D16(8)	*/

/* 4 Digits 7 Segment LED */
#define _7SEG_D5_INDEX			8	/* Segment 編號D5 (1)	*/
#define _7SEG_D6_INDEX			4	/* Segment 編號D6 (2)	*/
#define _7SEG_D7_INDEX			2	/* Segment 編號D7 (3)	*/
#define _7SEG_D8_INDEX			1	/* Segment 編號D8 (4)	*/
#define _7SEG_ALL			(_7SEG_D5_INDEX|_7SEG_D6_INDEX|_7SEG_D7_INDEX|_7SEG_D8_INDEX)

#define _7SEG_MODE_PATTERN		0	/* 自己指定值來顯示,1 : 亮, 0 : 不亮    */
						/* 每個Segment佔用1個byte,  共4個bytes  */
						/* Segment A : bit 0		        */
						/* Segment H : bit 7	    		*/			
						/* D5(1)是High byte,  D8(4)是low  byte  */		
						/* D5(1)是High byte,  D8(4)是low  byte  */
#define _7SEG_MODE_HEX_VALUE		1	/* 使用Hexadecimal, 當做輸入資料        */
						/* 每個佔用1個nibble,  共2個bytes       */
						/* D5(1)是high nibble of the 3th byte   */
						/* D8(4)是low  nibble of the low byte   */		
typedef struct _7Seg_Info {			
	unsigned char	Mode ;			/* 設定輸入的資料模式 			*/
	unsigned char	Which ;			/* 指定資料將設定那個Segment,		*/	 
						/* 使用or的值來指定Segment 		*/
						/* 如果不是全部則資料 			*/
						/* 最右邊的Segment的資料在最低的byte    */
						/* or nibble 				*/		
						/* 例如 : D5, D8顯示58 			*/
						/* Mode  = _7SEG_MODE_HEX_VALUE 	*/
						/* Which = _7SEG_D5_INDEX | _7SEG_D8_INDEX */
						/* Value = 0x58 			*/
						
	unsigned long	Value;			/* 各種Mode的輸入值 */
} _7seg_info_t ;

/* LCD specific ioctls 			*/
/* 清除LCD上的資料並且游標回到左上角 	*/
#define LCD_IOCTL_CLEAR			LCD_IO(  0x0)
/* 寫字元到LCD 上*/
#define LCD_IOCTL_WRITE			LCD_IOW( 0x01,	lcd_write_info_t)	
#define LCD_IOCTL_CUR_ON		LCD_IO(  0x02)
#define LCD_IOCTL_CUR_OFF		LCD_IO(  0x03)
#define LCD_IOCTL_CUR_GET		LCD_IOR( 0x04,	lcd_write_info_t)
#define LCD_IOCTL_CUR_SET		LCD_IOW( 0x05,	lcd_write_info_t)
#define LCD_IOCTL_DRAW_FULL_IMAGE	LCD_IOW( 0x06,  lcd_full_image_info_t)


/* LED specific ioctls			*/
/* 設定8個LED Lamps, Low byte 值為有效 	*/
#define LED_IOCTL_SET			LCD_IOW( 0x40,	unsigned short)
/* 點亮單一個LED lamp */	
#define LED_IOCTL_BIT_SET		LCD_IOW( 0x41,	unsigned short)
/* 熄滅單一個LED lamp*/
#define LED_IOCTL_BIT_CLEAR		LCD_IOW( 0x42,	unsigned short)

/* DIP specific ioctls		*/
/* 當Switch調到ON時所傳回值為 */
/* 讀取DIP SW的狀態,bit 0是1 bit 7是8 */
#define DIPSW_IOCTL_GET         LCD_IOR( 0x50,	unsigned short)   

/*  Keypad specific ioctls 	*/
/* 傳回按鍵的值 */
#define KEY_IOCTL_GET_CHAR		LCD_IOR( 0x60,	unsigned short)
/* 等待到有按鍵, 才傳回值*/
#define KEY_IOCTL_WAIT_CHAR		LCD_IOR( 0x61,	unsigned short)
/* 檢查是否有輸入資料 */
#define KEY_IOCTL_CHECK_EMTPY	LCD_IOR( 0x62, 	unsigned short)
/* 清除keybuffer 所有資料*/
#define KEY_IOCTL_CLEAR			LCD_IO(  0x63)
/* 取消等待輸入資料 */	
#define KEY_IOCTL_CANCEL_WAIT_CHAR	LCD_IO(  0x64)

/* 7SEG LED specific ioctls 	*/
/* 啟動 7 Segment LED */
#define _7SEG_IOCTL_ON			LCD_IO(  0x70)	
/* 關閉 7 Segment LED */
#define _7SEG_IOCTL_OFF			LCD_IO(  0x71)
/* 設定 7 Segment LED*/
#define _7SEG_IOCTL_SET			LCD_IOW( 0x72,	_7seg_info_t)

typedef struct _Timer_Info {			
	unsigned char	tick_sec ;			
	unsigned char	tick_min ;			 
	unsigned char	tick_hour ;		

} _Timer_info_t ;

// get creator timer
#define _TIME_IOCTL_GET			LCD_IOR(0x73, _Timer_info_t)

// set creator timer
#define _TIME_IOCTL_SET			LCD_IOW(0x74,	_Timer_info_t)


/*************************************************************************
EEPROM
*************************************************************************/
typedef struct _EEPROM_Info {			
	unsigned char	addr ;			
	unsigned char	data ;			 	
} _EEPROM_info_t ;

#define _EEPROM_IOCTL_GET      LCD_IOR(0x80, unsigned short)
#define _EEPROM_IOCTL_SET      LCD_IOW(0x81, unsigned short)


#endif // _CREATOR_S3C2410_LCD_H_ 

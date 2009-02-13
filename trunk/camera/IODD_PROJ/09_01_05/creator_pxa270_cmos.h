/*
 * cmos driver for Creator
 *
 *
 * Copyright (C) 2004 by Microtime Computer Inc.
 *
 * Linux kernel version history:
 * Version   : 1.00
 * History
 *   1.0.0 : Programming start (03/05/2004) -> SOP
 *
 */
 
#ifndef _CREATOR_S3C2410_CMOS_H_ 
#define _CREATOR_S3C2410_CMOS_H_ 

//#include <linux/config.h>
#if defined(__linux__)
#include <asm/ioctl.h>		/* For _IO* macros */
#define CCM_IOCTL_NR(n)	     		_IOC_NR(n)
#elif defined(__FreeBSD__)
#include <sys/ioccom.h>
#define CCM_IOCTL_NR(n)	     		((n) & 0xff)
#endif


#define CCM_MAJOR_NUM			110
#define CCM_IOCTL_MAGIC			CCM_MAJOR_NUM
#define CCM_IO(nr)			_IO(CCM_IOCTL_MAGIC,nr)
#define CCM_IOR(nr,size)		_IOR(CCM_IOCTL_MAGIC,nr,size)
#define CCM_IOW(nr,size)		_IOW(CCM_IOCTL_MAGIC,nr,size)
#define CCM_IOWR(nr,size)		_IOWR(CCM_IOCTL_MAGIC,nr,size)

// function headers


#define CMOS_SIZE_MODE_160_120		0
#define CMOS_SIZE_MODE_320_240		1

typedef enum {				/* CCM 狀態			 */
    CMOS_ON=0, 				/* 起動CCM		   	 */	
    CMOS_OFF, 				/* 關閉CCM		   	 */
    CMOS_IMG_READY,			/* CCM已經起動且影像資料準備好   */
    CMOS_IMG_EMPTY			/* CCM已經起動但影像資料沒準備好 */
} cmos_status_e ;	

typedef struct CMOS_INFO {
	unsigned char  command;		/* PC 送來的command, 		  */
					/* 因為看不懂Linux之CCM_IOWR...   */
	unsigned char  ImageSizeMode ; 	/* 影像模式大小			  */ 
	unsigned short ImageWidth;	/* 當執行IOCTL時且command=CMOS_ON */
					/* 傳回的影像寬與高		  */
	unsigned short ImageHeight;	        
	unsigned char  HighRef ;	/* 影響曝光時間,愈大時間愈長整體亮度愈亮 */
	unsigned char  Status;		/* CCM 目前狀態			 */	
} cmos_info_t;



/* CCM specific ioctls 			*/
#define CMOS_ON				CCM_IOWR( 0x00, cmos_info_t)
#define CMOS_OFF			CCM_IO(   0x01)
#define CMOS_PARAMETER			CCM_IOW(  0x02, cmos_info_t)
#define CMOS_GET_STATUS			CCM_IOR(  0x03, cmos_info_t)

#define PC_CMOS_ON			0
#define PC_CMOS_OFF			1
#define PC_CMOS_PARAMETER		2
#define PC_CMOS_GET_STATUS		3

#endif // _CREATOR_S3C2410_CMOS_H_ 


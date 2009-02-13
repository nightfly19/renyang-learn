// --------------------------------------------------------------------
//
//   Title     :  creator-pxa270-cmos.c
//             :
//   Library   :
//             :
//   Developers:  MICROTIME MDS group
//             :
//   Purpose   :  Driver for CMOS
//             :
//   Limitation:  .
//             :
//   Note      : 1. kmalloc() max size = 128Kb-16 on linux
//             :
//			   :
// --------------------------------------------------------------------
//   modification history :
// --------------------------------------------------------------------
//   Version| mod. date: |
//   V1.00  | 06/06/2006 | First release
//   V1.01  | 06/19/2006 | 
// --------------------------------------------------------------------
//
// Note:
//
//       MICROTIME COMPUTER INC.
//
//
/*************************************************************************
Include files
*************************************************************************/
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/sched.h>	
#include <linux/slab.h>
#include <linux/version.h>	
#include <asm/param.h>	
#include <asm/uaccess.h>

#include    <asm/arch/lib/creator_pxa270_core.h>
#include	<asm/arch/lib/creator_pxa270_cmos.h>


/*************************************************************************
Constant define
*************************************************************************/

/* ****** Debug Information ******************************************** */
//#define DEBUG
#ifdef DEBUG
#define MSG(string, args...) printk("<1>%s(), line=%d," string, __FUNCTION__, __LINE__, ##args)
#else   
#define MSG(string, args...)
#endif



/* ****** Linux Information ******************************************** */
#if LINUX_VERSION_CODE < 0x020100
#define GET_USER(a,b)   a = get_user(b)
#else
#include <asm/uaccess.h>
#define GET_USER(a,b)   get_user(a,b)
#endif



/* ****** Module Information ******************************************* */
#define MODULE_VERSION_INFO			"1.01"
#define MAX_MINORS                  1

#define MODULE_NAME					"CCM_CREATOR"
#define	MAJOR_NUM					110
#define COPYRIGHT					"Copyright (C) 2006, Microtime Computer Inc."
#define MODULE_AUTHOR_STRING		"Microtime Computer Inc."
#define MODULE_DESCRIPTION_STRING	"Creator CCM module"



/* ****** CMOS Information ********************************************** */
#define CPATURE_PERIOD			(HZ/5)    	// 200ms 
#define CCM_OK					1
#define CCM_IMG_ERR				-1
#define CCM_SYN_ERR				-2

#define WAIT_COUNT      		300000
#define GET_CCM_DATA(x) 		{x = CCM_DATA;udelay(2);}

#define Delay(x)	mdelay(x)

enum {NORMAL_MODE=0x0 , MINUS_MODE=0x1,  PLUS_MODE=0x2,   FPN_MODE=0x3};

typedef struct  {
	
	// Capture info
	int	Status ;
	int	nCapturing;
	int	Stop_Capturing;
	int 	ImageLineGetIndex;	
	UC	bySyncCCM, byFilterNoise;	
	
	// Image buffer
	UC      *pbyBmpBuffer;
	UC      *pbyRawBuffer;
	UC      *pbyFpnBuffer;	
	UC		*pbyBackupRawBuffer;
	
	// CMOS attribute
	UL	dwImageSize;
	int 	nImgWidth, nImgHeight, nHighRef ;		

} ST_CMOSInfo ;




/*************************************************************************
Function prototypes
*************************************************************************/
static int SensorExposureMode (UC Mode, int ImgMode);
static int ReadPICFrameData (UC *pImageBuffer, int imgW, int imgH);
static int CheckEVC250KU (UC *pImgBuffer, int ImgWidth, int ImgHeight);
static int ReadPICFrame (UC *pmidImageBuffer, int imgW, int imgH, UC *pTmpImageBuffer, UC *pNoiseImageBuffer);
static int ReadPICFPN (UC *pmidBuffer, int imgW, int imgH, UC *ptmpBuffer);
static int ReadAvgFPN (UC *pmidBuffer, int imgW, int imgH, UC *ptmpBuffer);
static int CaptureFPN (int ImgWidth, int ImgHeight, UC *ptmpBuffer, UC *pbyFpnBuffer);
static int CaptureImage_CCM (void);
static UC  GoCCM (void);
static int CaptureImage(void);
static UC  ProcessAttribute(cmos_info_t *inf);
static void OptimizationDelay(void) {}	// for Optimization



/*************************************************************************
Variable define
*************************************************************************/ 
extern creator_io_t creator_io ;
static int        	MaxRef=0;
static int        	MinRef=255;
static int        	deltaRef,MaxGrayVal;
static UC	   		byHighRef=80; //Range : 0~255
static int        	OffsetFpn;
static int        	IsFPN = FALSE ;
static UC	   		byImageSizeMode;
static UC 	        byPreExposureMode;

static ST_CMOSInfo  st_CMOSInfo;
static DECLARE_WAIT_QUEUE_HEAD(wq_read);
/* ************************************************************************** */



static int 
SensorExposureMode (UC Mode, int ImgMode)
{
/*
Procecure :
                1. send exposure mode.
                2. Set nDstrb low, so CCM Wait low to high then nDstrb to High
                3. send ImgSize mode
                4. wait nWait is high
                5. Set nDstrb high, so CCM Wait low to high then nDstrb to High
*/
        unsigned long flags;
        volatile UC temp;
	
        //send exposure mode
        //
        //Set nDstrb to low
        //
        spin_lock_irqsave(&creator_io.creator_lock, flags);          
        creator_io.cpld_ctrl = ((creator_io.cpld_ctrl & (~0xc)) | ((Mode&3)<<2));        
        CPLD_CTRL = creator_io.cpld_ctrl;
        spin_unlock_irqrestore(&creator_io.creator_lock, flags);        
        
        GET_CCM_DATA(temp);
 
        OptimizationDelay();
               
        //send ImgSize mode
        //Set nDstrb to low
        //
        spin_lock_irqsave(&creator_io.creator_lock, flags);          
        creator_io.cpld_ctrl = ((creator_io.cpld_ctrl & (~0xc)) | ((ImgMode&0x3)<<2));
        CPLD_CTRL = creator_io.cpld_ctrl ;
        spin_unlock_irqrestore(&creator_io.creator_lock, flags);
                
        GET_CCM_DATA(temp);
        
        MSG("\n");
        
        return (CCM_OK);
}
/* ************************************************************************** */



static int 
ReadPICFrameData (UC *pImageBuffer, int imgW, int imgH)
{
        UL      cnt ;
        UC      *p, *pLine ;
        volatile UC temp;
        int		line, col, HalfImgW = imgW>>1;

        // check frame Ready ? --> It means "Ready" signal is high ?
        //    
        cnt = WAIT_COUNT;
        while (cnt)
        {
            temp = CPLD_STATUS ^ 0x10;
            if (temp & 0x10) break;
            udelay(1);
            --cnt;
        }
        if (!cnt){
			MSG("\n");
			return (CCM_SYN_ERR);
		}        

		//The output data after ADC means :
		// 0 Voltage --> White pixel (gray value=0)
		// 5 Voltage --> Black pixel (gray value=1)
		// So, it needs to convert the output data
		pLine = pImageBuffer; 
		for (line=0; line < imgH; ++line)
		{
     	
			p = pLine;
			for (col=0; col < HalfImgW; ++col) //Read Out the even pixels
			{
				GET_CCM_DATA(*p);
				p+=2;
			}
			p = pLine + 1;
			for (col=0; col < HalfImgW; ++col) //Read Out the odd pixels
			{
				GET_CCM_DATA(*p);
				p+=2;                
			}
			pLine += imgW;
		}

		// check frame end ? --> It means "Ready" signal is low ?
		//
		cnt = WAIT_COUNT;
		while (cnt)
		{
			temp = CPLD_STATUS ^ 0x10;
			if (temp & 0x10){
				GET_CCM_DATA(temp);
				cnt--;
			}
			else break;
		}
		if (!cnt)
		{
			MSG("\n");
			return (CCM_SYN_ERR);  
		}     
 
		MSG("\n");
		return (CCM_OK);
}
/* ************************************************************************** */



static int 
CheckEVC250KU (UC *pImgBuffer, int ImgWidth, int ImgHeight)
{
        unsigned long   cnt;
        int             i, nRet;  
        volatile UC 	temp;                   

		// Syn Color Image Sensor. 
		// check frame Ready ? --> It means "Ready" signal is high ?
		//  
		for (cnt =0; cnt < 500;  cnt++){
			temp = CPLD_STATUS ^ 0x10;
			if (temp & 0x10){
				GET_CCM_DATA(temp);            
				break ;
			}
			Delay(10);
		}
		if (cnt == 500)
		{
			MSG("\n");
			return (CCM_SYN_ERR);
		}

		// Read image pixel.(add dumy pixel)
		//
		cnt = (508 * 480) + 5;
		while (cnt)
		{
			temp = CPLD_STATUS ^ 0x10;
			if (temp & 0x10){
				GET_CCM_DATA(temp);
				cnt--;
			}
			else break;
		}
		if (!cnt)
		{
			MSG("\n");
			return (CCM_SYN_ERR);
		}

		byPreExposureMode = NORMAL_MODE;  
		nRet = SensorExposureMode(NORMAL_MODE, byImageSizeMode);
		if (nRet != CCM_OK)
		{
			MSG("\n");
			return (nRet);
		}
		for (i =0; i < 1; i++){
			Delay(50);
			nRet = ReadPICFrameData(pImgBuffer, ImgWidth, ImgHeight);
			if (nRet != CCM_OK)
			{
				MSG("\n");
				return (nRet);
			}
			nRet = SensorExposureMode(NORMAL_MODE, byImageSizeMode);
			if (nRet != CCM_OK)
			{
				MSG("\n");
				return (nRet);
			}
		}
		Delay(30);        
        
		MSG("\n");
		return (CCM_OK);
}
/* ************************************************************************** */



static int 
ReadPICFrame (UC *pmidImageBuffer, int imgW, int imgH, UC *pTmpImageBuffer, UC *pNoiseImageBuffer)
{
        long    summ;      //Used to do auto exposure control
        int     line, col, tmp, nRet;
        UC      *p, *p2, *midLine, *tmpLine, *noiseLine;
        UC      *pNoise;

        nRet = ReadPICFrameData(pTmpImageBuffer, imgW, imgH);
        if (nRet != CCM_OK){	
            MSG("\n");
            return (nRet);
        }

        nRet = SensorExposureMode(byPreExposureMode, byImageSizeMode);
        if (nRet != CCM_OK){
            MSG("\n");
            return (nRet);  
        }  
                
        midLine = pmidImageBuffer;
        tmpLine = pTmpImageBuffer;
        noiseLine = pNoiseImageBuffer;       
        summ = 0; //Used to do auto exposure control                
        for (line=0; line < imgH; ++line){
            p2 = tmpLine;
            p = midLine;
            pNoise = noiseLine;
            for (col=0; col < imgW; ++col){
                if (IsFPN){			
                    tmp = *pNoise++;
                    tmp = tmp + deltaRef - (int)(*p2);
                    if (tmp >= MaxGrayVal)
                        tmp = MaxGrayVal;
                    else if (tmp <=0 )
                        tmp = 0;
               }
               else
                   tmp = (*p2)^0xff;
               *p = (UC)tmp;
               summ += tmp;
               ++p2;   ++p;
           }
           midLine += imgW;
           tmpLine += imgW;
           noiseLine += imgW;                        
        }
        summ = (int)(summ/imgW/imgH);
	
        if (summ <= (byHighRef-10))     byPreExposureMode = PLUS_MODE;//MINUS_MODE;
        else if (summ > byHighRef)      byPreExposureMode = MINUS_MODE;//PLUS_MODE;
        else                            byPreExposureMode = NORMAL_MODE;

        MSG("\n");
        return (CCM_OK);
}
/* ************************************************************************** */



static int 
ReadPICFPN (UC *pmidBuffer, int imgW, int imgH, UC *ptmpBuffer)
{
        int	nRet ;
	
        MaxRef = 0;
        MinRef = 255;

        nRet = ReadPICFrameData(pmidBuffer, imgW, imgH);
        if (nRet != CCM_OK){

            MSG("\n");
            return (nRet);
        }
        MSG("\n");

        return (SensorExposureMode(FPN_MODE, byImageSizeMode));
}
/* ************************************************************************** */



static int 
ReadAvgFPN (UC *pmidBuffer, int imgW, int imgH, UC *ptmpBuffer)
{
        int     tmp, line, col, nRet;
        UC      *p, *p2, *tmpLine, *midLine;

        nRet = ReadPICFrameData(ptmpBuffer, imgW, imgH);
        if (nRet != CCM_OK){	
            MSG("\n");
            return (nRet);
        }
            
        tmpLine = ptmpBuffer;    
        midLine = pmidBuffer;
        for (line=0; line < imgH; ++line){

            p2 = tmpLine;
            p = midLine;
            for (col=0; col < imgW; ++col){
                tmp = *p;
                tmp = (tmp + (int)(*p2)) >> 1;
                *p = (UC)tmp;
                ++p2;   ++p;
                if (tmp > MaxRef) MaxRef = tmp;
                if (tmp < MinRef) MinRef = tmp;
            }
            tmpLine += imgW ;
            midLine += imgW ;
        }
        nRet = SensorExposureMode(FPN_MODE, byImageSizeMode);
        if (nRet != CCM_OK){
            MSG("\n");
            return (nRet);
        }

        deltaRef = MaxRef - MinRef;
        if (deltaRef >= OffsetFpn)      MaxGrayVal = 255 - OffsetFpn;
        else                            MaxGrayVal = 255 - deltaRef;
        deltaRef = 255 - MaxRef;

        MSG("\n");
        return (CCM_OK);
}
/* ************************************************************************** */



static int 
CaptureFPN (int ImgWidth, int ImgHeight, UC *ptmpBuffer, UC *pbyFpnBuffer)
{
        int    i, IsCCMOK;

        IsCCMOK = ReadPICFPN(pbyFpnBuffer, ImgWidth, ImgHeight, ptmpBuffer);

        if (IsCCMOK == CCM_OK){
            Delay(50);        	
            IsCCMOK = ReadPICFPN(pbyFpnBuffer, ImgWidth, ImgHeight, ptmpBuffer);
        }
        
        if (IsCCMOK == CCM_OK){
            for (i=0; i < 3 && IsCCMOK == CCM_OK; i++){
                Delay(50);
                IsCCMOK = ReadAvgFPN(pbyFpnBuffer, ImgWidth, ImgHeight, ptmpBuffer);
                if (IsCCMOK != CCM_OK)
                    break ;
            }
        }
        
        MSG("\n");
        return (IsCCMOK);
}
/* ************************************************************************** */



static int 
CaptureImage_CCM (void)
{
        int	nRet ;
        UC	ImgMode, *pImageRawData;
	
        // 設定Image Size Mode for Exposure Mode
        //
        if (st_CMOSInfo.nImgWidth == 160 && st_CMOSInfo.nImgHeight == 120)
            ImgMode = 0;
        else if (st_CMOSInfo.nImgWidth == 320 && st_CMOSInfo.nImgHeight == 240)
            ImgMode = 1;	    
        else if (st_CMOSInfo.nImgWidth == 508 && st_CMOSInfo.nImgHeight == 480)
            ImgMode = 2;	    
        else{
            MSG("\n");
            return (CCM_IMG_ERR);	
        }    

        byImageSizeMode = ImgMode ;	
        byHighRef = st_CMOSInfo.nHighRef;
        IsFPN = st_CMOSInfo.byFilterNoise;
        if (st_CMOSInfo.bySyncCCM == TRUE){
            nRet = CheckEVC250KU(st_CMOSInfo.pbyRawBuffer, st_CMOSInfo.nImgWidth, st_CMOSInfo.nImgHeight);
            if (nRet != CCM_OK)	{
                MSG("\n");
                return (nRet);	
            }
            nRet = CaptureFPN(st_CMOSInfo.nImgWidth, st_CMOSInfo.nImgHeight, st_CMOSInfo.pbyBmpBuffer, st_CMOSInfo.pbyFpnBuffer);
            if (nRet != CCM_OK){        
                MSG("\n");
                return(nRet);
            }
        }		 
	 
        pImageRawData = st_CMOSInfo.pbyRawBuffer; 
        nRet = ReadPICFrame(pImageRawData, st_CMOSInfo.nImgWidth, st_CMOSInfo.nImgHeight, st_CMOSInfo.pbyBmpBuffer, st_CMOSInfo.pbyFpnBuffer);
	
        MSG("\n");
        return (nRet);    
}
/* ************************************************************************** */



static int 
CaptureImage (void)
{
        int	IsCCMOK ;       
        UC	*pdest;

        // 取得CCM Image
        //
        IsCCMOK = CaptureImage_CCM();	
        if (IsCCMOK != CCM_OK){
            MSG("\n");
            return (FALSE);
        }
        // 只要一次同步CCM即可.但是如果CaptureImage_CCM失敗.需要再次同步
        st_CMOSInfo.bySyncCCM = 0;
        //wake_up_interruptible(&wq_read);
        if (st_CMOSInfo.Status == CMOS_IMG_EMPTY || st_CMOSInfo.Status == CMOS_ON){
            pdest = st_CMOSInfo.pbyBackupRawBuffer;	        	
            memcpy(pdest, st_CMOSInfo.pbyRawBuffer, st_CMOSInfo.dwImageSize);	
            st_CMOSInfo.Status = CMOS_IMG_READY;
            wake_up_interruptible(&wq_read);            
           //printk("copy CCM data to buffer\n");        	
        }        	

        MSG("\n");
        return (TRUE);
}
/* ************************************************************************** */



static void 
ResetCMOS (void)
{
        st_CMOSInfo.Status = CMOS_OFF;
        st_CMOSInfo.nCapturing = UM;
        st_CMOSInfo.bySyncCCM = 1;	
}
/* ************************************************************************** */



static UC 
GoCCM (void)
{
        int	Ret ;	

        MSG("\n");
	
        if (st_CMOSInfo.nCapturing){
            return (OK);
        }
	    
        if (st_CMOSInfo.Stop_Capturing){
            return (OK);
        }
	    
        st_CMOSInfo.nCapturing = OK;    	
        Ret = CaptureImage() ;
        st_CMOSInfo.nCapturing = UM;          
        
        if (Ret != OK){
            st_CMOSInfo.Status = CMOS_OFF;			
            ResetCMOS();

            MSG("\n");              
            return (-EBUSY);	 	     
        }

        MSG("\n");        
        return (OK);
}
/* ************************************************************************** */



static UC 
ProcessAttribute (cmos_info_t *info)
{
        UL  	dwImageSize ;
        int		w, h;
        UC  	Ret = OK;	
	
        // ImageSize mode = 0..2
        //
        // 
        //printk("Image Size Mode = %d\n", info->ImageSizeMode);
        if (info->ImageSizeMode > 2)
            return (-EINVAL);	
	
        switch (info->ImageSizeMode){
        case 0 :	   
            w = 160; h = 120;
            break;			
        case 1 :	   
            w = 320; h = 240;
            break;	   
        case 2 :
            w = 508; h = 480;
            break; 	   
        default :
            return (-EINVAL);	    
        }
        info->ImageWidth = w; info->ImageHeight = h;
        dwImageSize = w * h;        
                
        if (w != st_CMOSInfo.nImgWidth || h != st_CMOSInfo.nImgHeight){
            kfree(st_CMOSInfo.pbyBmpBuffer); //pbyBmpBuffer = NULL;
            kfree(st_CMOSInfo.pbyRawBuffer); //pbyRawBuffer = NULL;
            kfree(st_CMOSInfo.pbyFpnBuffer); //pbyFpnBuffer = NULL;
            kfree(st_CMOSInfo.pbyBackupRawBuffer); //pbyBackupRawBuffer = NULL;
        
            st_CMOSInfo.pbyBmpBuffer = (UC*)kmalloc(dwImageSize , GFP_KERNEL);
            st_CMOSInfo.pbyRawBuffer = (UC*)kmalloc(dwImageSize , GFP_KERNEL);
            st_CMOSInfo.pbyFpnBuffer = (UC*)kmalloc(dwImageSize, GFP_KERNEL);
            st_CMOSInfo.pbyBackupRawBuffer = (UC*)kmalloc(dwImageSize, GFP_KERNEL);
            
            st_CMOSInfo.nImgWidth = w;
            st_CMOSInfo.nImgHeight = h;            
        }    
        st_CMOSInfo.dwImageSize = dwImageSize;
        //printk("Image size = %d\n", st_CMOSInfo.dwImageSize);
        st_CMOSInfo.nHighRef = info->HighRef;
        st_CMOSInfo.byFilterNoise = 0;         
        ResetCMOS();
        
        if (st_CMOSInfo.pbyBmpBuffer && st_CMOSInfo.pbyRawBuffer && st_CMOSInfo.pbyFpnBuffer &&  
            st_CMOSInfo.pbyBackupRawBuffer){
            //printk("Go to GOCCM\n");
            //Ret = GoCCM();
        }
        else{
            printk("kmalloc error!\n");	
            return (UM);	
        }
    
        return (Ret);
}
/* ************************************************************************** */



static struct timer_list auto_capture_timer;
void 
AutoCaptureImage(void)
{
        MSG("\n"); 	
        del_timer(&auto_capture_timer);
        if (GoCCM() != OK){	    
            MSG("\n"); 			
            return ;	   	
        }	
        if (st_CMOSInfo.Status != CMOS_OFF){ 
            MSG("\n"); 	
            auto_capture_timer.expires = jiffies + CPATURE_PERIOD;
            add_timer(&auto_capture_timer);	
        }
}
/* ************************************************************************** */



static int 
StartCMOS (cmos_info_t *info)
{
        if (ProcessAttribute(info) != OK){
            printk("Alloc memory Error\n");
            return (-EBUSY);	
        }
	
        if (GoCCM() != OK){
            printk("GO CCM Error\n");	
            return (-EBUSY);	
        }
        printk("CMOS starting\n");        
        
        st_CMOSInfo.Status = CMOS_ON;
        st_CMOSInfo.ImageLineGetIndex = 0;	
	    
        auto_capture_timer.expires = jiffies + HZ/5;
        auto_capture_timer.function = (void *)AutoCaptureImage;
        add_timer(&auto_capture_timer);	
	
        return (OK);	
}
/* ************************************************************************** */





static void 
cpld_hwsetup (void)
{
}	
/* ************************************************************************** */



static int 
drv_cmos_open (struct inode *inode, struct file *filp)
{        
        return(0);
}
/* ************************************************************************** */



static int 
drv_cmos_release (struct inode *inode, struct file *filp)
{
        
        return 0;
}
/* ************************************************************************** */



static ssize_t 
drv_cmos_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
/*
Ret 	= value : read data, 
	= 0	: EOF
*/	
        ssize_t	ret =0 ;
        int 	nNextImageFrame = FALSE;
        UC		*src; 
	
        if (st_CMOSInfo.Status == CMOS_OFF) return (-ENODATA);
	
        //printk("Status=%d Read %d f_pos %d ImageSize=%d\n", st_CMOSInfo.Status,count, *f_pos, st_CMOSInfo.dwImageSize);	
        if (st_CMOSInfo.Status == CMOS_IMG_EMPTY || st_CMOSInfo.Status == CMOS_ON){
            //ret = -ENODATA;
            interruptible_sleep_on(&wq_read);
            //goto out;
        }

        //避免讀取啟始位置超出一個影像Size.		
        if (*f_pos >= st_CMOSInfo.dwImageSize){
            goto out;
        }

        // 必免讀取資料長度超出一個影像Size	    
        if ((UL)(*f_pos + count) >= st_CMOSInfo.dwImageSize){
            // 當讀取完全部ImageSize後,下次讀取換下一個Frame.	
            count = st_CMOSInfo.dwImageSize - *f_pos; 	
            nNextImageFrame = TRUE;
        }		
        st_CMOSInfo.Stop_Capturing = OK;  
	
        //找出資料正確位置
        src = (UC*)(st_CMOSInfo.pbyBackupRawBuffer + *f_pos);
        if (copy_to_user((UC*)buf, (UC*)src, count)){
            ret = -EFAULT;
            goto out;		
        }
        *f_pos += count;
        ret = count;

        if (nNextImageFrame)	{
            st_CMOSInfo.Status = CMOS_IMG_EMPTY;
            /* 重新設定f_pos 因為資料以經讀完*/
            *f_pos = 0;
            //printk("copy one page next frmae\n", count);	     
        }
out:
        //printk("\nread size = %d\n", ret);
        st_CMOSInfo.Stop_Capturing = UM; 	
	
        return (ret);		
}
/* ************************************************************************** */



static ssize_t 
drv_cmos_write (struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
        return (0);
}
/* ************************************************************************** */



static int 
drv_cmos_ioctl (struct inode *inode, struct file *filep, unsigned int command, unsigned long argAddress)
{
        cmos_info_t 	info ;	
	    void            *arg ;	    
	    int             arg_size ;
	    int             dir;        
	    int             rc = 0, ret;		

        /*
         * 分離type如果遇到錯誤的cmd, 就直接傳回ENOTTY
         */
        if (_IOC_TYPE(command) != CCM_IOCTL_MAGIC) return (-ENOTTY);
        
        arg = (void*)argAddress;        
	    arg_size = _IOC_SIZE(command);
	    dir = _IOC_DIR(command);	        
	
        switch (command) {
        case CMOS_ON:{		
            //printk("Entry CMOS ON...\n");  
            if (copy_from_user(&info, arg, arg_size)){
                printk("CMOS ON parameter error\n");
                return (-EINVAL);
            }
            //printk("Start CMOS \n");   
            if ((ret = StartCMOS(&info)) < 0){
                printk("Start CMOS error\n");
                return 	(-EBUSY); 
            }		        
            if (copy_to_user(arg, &info, arg_size))              		
                return (-EINVAL);		    	
            break;
        }
		    
        case CMOS_OFF:		   						
            st_CMOSInfo.Status = CMOS_OFF;		
            break;	
	    
        case CMOS_GET_STATUS:	
            if (copy_from_user(&info, arg, arg_size)){
                return (-EINVAL);
            }
            info.Status = (unsigned char)st_CMOSInfo.Status;
	    	       
            if (copy_to_user(arg, &info, arg_size))              		
                return (-EINVAL);		    						
            break;		    				
	    
        case CMOS_PARAMETER : {
            cmos_info_t info ;
	    
            //printk("set parameter\n");
            if (copy_from_user(&info, arg, arg_size))
                return (-EINVAL);	     			
            st_CMOSInfo.nHighRef = info.HighRef;            
            break ;	
        }
        default:
            return (-ENOTTY);
        }
        
        return (rc);
}
/* ************************************************************************** */



/*
 *	Exported file operations structure for driver...
 */

struct file_operations	drv_ccm_fops =
{
    owner:      THIS_MODULE,    
	read:		drv_cmos_read,
	write:		drv_cmos_write,
	ioctl:		drv_cmos_ioctl,
	open:		drv_cmos_open,
	release:	drv_cmos_release,


};



static struct cdev cmos_cdev = {
	.kobj	=	{.name = MODULE_NAME, },
	.owner	=	THIS_MODULE,
};
/* ************************************************************************** */



static int __init 
drv_cmos_init (void)
{
        struct  cdev   *pcdev;      
        dev_t   devno;    
        int	    error;	
                   
        ResetCMOS();	
	
	    devno = MKDEV(MAJOR_NUM, 0);	    
        if (register_chrdev_region(devno, MAX_MINORS, MODULE_NAME)){    
            printk("<1>%s: can't get major %d\n", MODULE_NAME, MAJOR_NUM);			    
            return (-EBUSY);
        }		    
        pcdev = &cmos_cdev;		    
    	cdev_init(pcdev, &drv_ccm_fops);
	    pcdev->owner = THIS_MODULE;    
    	
        /* Register lcdtxt as character device */
        error = cdev_add(pcdev, devno, MAX_MINORS);
        if (error) {
            kobject_put(&pcdev->kobj);
            unregister_chrdev_region(devno, MAX_MINORS);

            printk(KERN_ERR "error register %s device\n", MODULE_NAME);
            
            return (-EBUSY);
        }        
        printk("<1>%s: Version : %s %s\n", MODULE_NAME, MODULE_VERSION_INFO, COPYRIGHT);   	


        /* Hardware specific initialization */
        cpld_hwsetup();
	
        printk("<1>CMOS Active\n");
        init_timer(&auto_capture_timer);	
        
        return (0);
}
/* ************************************************************************** */



static void __exit 
drv_cmos_cleanup (void)
{	
        st_CMOSInfo.Status = CMOS_OFF;
        while (st_CMOSInfo.nCapturing || st_CMOSInfo.Stop_Capturing) {}	

        kfree(st_CMOSInfo.pbyBmpBuffer); //pbyBmpBuffer = NULL;
        kfree(st_CMOSInfo.pbyRawBuffer); //pbyRawBuffer = NULL;
        kfree(st_CMOSInfo.pbyFpnBuffer); //pbyFpnBuffer = NULL;
        kfree(st_CMOSInfo.pbyBackupRawBuffer); //pbyBackupRawBuffer = NULL;
	
        wake_up_interruptible(&wq_read);
        cdev_del(&cmos_cdev);	
       	unregister_chrdev_region(MKDEV(MAJOR_NUM, 0), MAX_MINORS);
       	
        printk("<1>%s: removed\n", MODULE_NAME);	
}	
/* ************************************************************************** */



module_init(drv_cmos_init);
module_exit(drv_cmos_cleanup);

MODULE_AUTHOR(MODULE_AUTHOR_STRING);
MODULE_DESCRIPTION(MODULE_DESCRIPTION_STRING);



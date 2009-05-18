#include <stdio.h>
#include <cv.h>
#include <highgui.h>

int main()
{
    char FileName[10]="rain.jpg";

    // 來源指標
    IplImage *src = 0;

    // 目的指標
    IplImage *dst = 0;

    // 縮小倍數
    float scale = 0.618;

    // 目標圖像的大小
    CvSize dst_cvsize;

    // 載入圖檔
    src = cvLoadImage("rain.jpg",1);

    // 設定目標寬為100 pixel
    dst_cvsize.width = 100;
    // 同比例縮小
    dst_cvsize.height = src->height * ((float) dst_cvsize.width/src->width);

    // 建構目標圖像
    dst = cvCreateImage(dst_cvsize,src->depth,src->nChannels);

    // 縮小來源圖到目標圖像
    cvResize(src,dst,CV_INTER_LINEAR);

    // 列印出原圖與縮小後圖的大小
    printf("the original size is %i\n",src->imageSize);
    printf("the scaled size is %i\n",dst->imageSize);

    cvNamedWindow("src",CV_WINDOW_AUTOSIZE);

    cvNamedWindow("dst",CV_WINDOW_AUTOSIZE);

    // 顯示來源圖像
    cvShowImage("src",src);

    // 顯示目標圖像
    cvShowImage("dst",dst);

    // 等待用戶反應
    cvWaitKey(-1);

    // 釋放來源圖占用的記憶體
    cvReleaseImage(&src);

    // 釋放目標圖占用的記憶體
    cvReleaseImage(&dst);

    return 0;
}

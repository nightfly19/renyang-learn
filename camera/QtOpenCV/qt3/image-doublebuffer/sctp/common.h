#ifndef COMMOM_H
#define COMMON_H

#define MAX_BUFFER 1024

#define MAXDATA 480*640*3

#define MY_PORT_NUM 10000

#define IMAGE_START "#image_start#"
#define IMAGE_END "#image_end#"
#define IMAGE_OK "#image_ok#"
#define IMAGE_ERROR "#image_error#"

struct image_matrix
{
        int height, width;
        char data[MAXDATA];
};

#endif

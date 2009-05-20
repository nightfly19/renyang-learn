#ifndef COMMOM_H
#define COMMON_H

#define MAX_BUFFER 1024

#define MAXDATA 480*640*3

#define MY_PORT_NUM 10000

#define START_IMAGE "#image_start#"
#define END_IMAGE "#image_end#"
#define SENDING_IMAGE "#image_sending#"
#define ERROR_IMAGE "#image_error#"

struct image_matrix
{
        int height, width;
        char data[MAXDATA];
};

#endif

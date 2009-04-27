#include <math.h>

#include "stream_common.h"

int sendFile(char *ip_addr, int max_channelsIn, int max_channelsOut, char* fileNameOriginal, int several_parts){
    int    fileNameLen;
    char** fileNames;
    int parts;

    int file_mtu = FILE_MTU;
    int mtu      = MTU;

    int sockfd;
    int port = PORT;
    struct sctp_sndrcvinfo sinfo;

    clock_t start, end;

    int i;
    int value;
  
    start=clock();
    if(!createSocketSend(&sockfd, port, ip_addr, &max_channelsIn, &max_channelsOut)){
        close(sockfd);
        return 2;
    }

    bzero(&sinfo, sizeof(sinfo));
    sinfo.sinfo_stream = max_channelsOut-1;

    printf("Splitting file. Please, wait.....\n");
    parts = fileSplit(&fileNames, fileNameOriginal, file_mtu);
    fileNameLen = (strlen(fileNameOriginal)+1);
    printf("Sending file.....\n");
    switch(parts){
    
       case 0: //Error
            printf("Error fileSplit\n");
            break;

       case 1: //Only one file
           if (fileSend(sockfd, max_channelsOut, fileNameOriginal, mtu)){
                printf("Error in fileSend\n");
                return 2;
            }
            value = EXIT;
            sctp_send(sockfd, &value, sizeof(int), &(sinfo), 0);
            break;

       default: //File in several parts
           if(several_parts > 0){
               i = several_parts;
           }else{
               i = 0;
           }

           do{
               if (fileSend(sockfd, max_channelsOut, fileNames[i], mtu)){
                   printf("Error in fileSend\n");
                   return 2;
               }
               
               i++;
               
               if(i==parts){
                   value = JOIN;
                   sctp_send(sockfd, &value, sizeof(int), &(sinfo), 0);
                   sctp_send(sockfd, &parts, sizeof(int), &(sinfo), 0);
                   sctp_send(sockfd, &fileNameLen, sizeof(int), &(sinfo), 0);
                   sctp_send(sockfd, fileNameOriginal, sizeof(char)*fileNameLen, &(sinfo), 0);
                   printf("PARTS: %i, NAME: %s\n", parts, fileNameOriginal);
               }
               else{
                   value = CONTINUE;
                   sctp_send(sockfd, &value, sizeof(int), &(sinfo), 0);
               }
               
           }while(i<parts);
           
           if(fileNames != NULL){
               for(i = 0; i<parts; i++)
                   if(fileNames[i] != NULL){
                       printf("Deleting Filename: %s\n", fileNames[i]);
                       //if( remove( fileNames[i] ) == -1 ) perror("remove");
                       free(fileNames[i]);
                   }
               free(fileNames);
           }
           break;
    }

    sinfo.sinfo_flags = SCTP_EOF;
    sctp_send(sockfd, NULL, 0, &(sinfo), 0);
    close(sockfd);
    end=clock();
    printf("TIME: %1.3fsg\n", ((double)(end-start))/((double)CLOCKS_PER_SEC));
    
    return(0);
}


int receiveFile(int max_channelsIn, int max_channelsOut){

    int                    client_sockfd;
    int                    controlChannel;
    int                    port = PORT;
    struct sockaddr_in     client_addr;
    struct sctp_sndrcvinfo sinfo;
    int                    flags;
    size_t                    len;

    char* fileName;
    int   fileNameLen;
    int   parts;

    clock_t start, end;

    int value;

    len = sizeof(struct sockaddr_in);

    start=clock();
    if( !createSocketReceive(&client_sockfd, port, &max_channelsOut, &max_channelsIn) ){
        printf("Error creating control socket");
        return 0;
    }
  
    controlChannel = max_channelsIn-1;
    do{
        //FILE_RECEIVE
        if(fileReceive(client_sockfd, max_channelsIn)){
            printf("Error in fileSend\n");
            return 2;
        }

        sctp_recvmsg(client_sockfd, &value, sizeof(int), (struct sockaddr *) &client_addr, &len, &sinfo, &flags);

        if(value == JOIN){
            sctp_recvmsg(client_sockfd, &parts, sizeof(int), (struct sockaddr *) &client_addr, &len, &sinfo, &flags);
            sctp_recvmsg(client_sockfd, &fileNameLen, sizeof(int), (struct sockaddr *) &client_addr, &len, &sinfo, &flags);
            fileName = (char*)malloc(fileNameLen*sizeof(char));
            sctp_recvmsg(client_sockfd, fileName, sizeof(char)*fileNameLen, (struct sockaddr *) &client_addr, &len, &sinfo, &flags);

            printf("Merging files. Please, wait.....\n");
            if(!mergeFiles(fileName, parts)){
                printf("Error in mergeFiles\n");
                return 2;
            }

            if(fileName != NULL) free(fileName);
        }

    }while(value != EXIT && value != JOIN);

    close(client_sockfd);
    end=clock();
    printf("TIME: %1.3fsg\n", ((double)(end-start))/((double)CLOCKS_PER_SEC));

    return 0;
}



void usage(int msg){
    switch(msg){
        //client
    case 1: printf("arguments: ip-addr istreams ostreams file {part_number}\n");
        break;
        //server
    case 2: printf("arguments: istreams ostreams\n");
        break;
    }
}

void setPos(unsigned char* left, unsigned char* right, int source){
    *right = 255 & source;
    *left = ( (65280 & source) >> 8);
}

int getPos(unsigned char left, unsigned char right){
    return (right + (left << 8));
}

int mergeFiles(char * fileName, int parts){
    unsigned char* buf2;
    long           size;
    FILE*          fp;
    FILE*          fpOriginal;

    char* fileNamePart;

    int i;

    fileNamePart = (char*)malloc(sizeof(char)*(strlen(fileName)+FILE_EXT_LEN));
    if(fileNamePart == NULL){
        perror("fileNamePart");
        return 0;
    }
        

    //open file-1 for write
    fpOriginal=fopen(fileName,"w");
    if(fpOriginal == NULL){
        if(fileNamePart != NULL) free(fileNamePart);
        perror("fpOriginal");
        return 0;
    }

    for(i=0; i< parts; i++){
        //create the name
        sprintf(fileNamePart, "%s.%i", fileName, i);
        //open file-2 for read
        fp=fopen(fileNamePart,"rb");
        if(fp == NULL){
            if(fileNamePart != NULL) free(fileNamePart);
            perror("fopen");
            return 0;
        }
        
        //get size of file-2
        fseek(fp,0,SEEK_END);
        size=ftell(fp);
        rewind(fp);

        //allocate buffer, read-in file-2
        buf2=(unsigned char*)malloc((size_t)size);
        if(buf2 == NULL){
            if(fileNamePart != NULL) free(fileNamePart);
            perror("malloc");
            return 0;
        }

        fread(buf2,size,1,fp);

        //write the buffer to (end of) file-1
        fwrite(buf2,size,1,fpOriginal);

        //clean-up and close
        free(buf2);
        fclose(fp);

        //remove the file
        if( remove( fileNamePart ) == -1 ) perror("remove");
    }
    
    fclose(fpOriginal);
    if(fileNamePart != NULL) free(fileNamePart);

    return 1;
}

int createSocketReceive(int *client_sockfd, int port, int *maxInstreams, int *numOstreams){
    int                         server_sockfd;
    struct sockaddr_in          serv_addr, client_addr;
    struct sctp_initmsg         initmsg;
    struct sctp_status          status;
    struct sctp_event_subscribe events;
    size_t                      len;

    //create endpoint
    server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if(server_sockfd < 0){
        perror("socket");
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if(bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1){
        perror("sctp bind");
        return 0;
    }

    memset(&initmsg, 0, sizeof(initmsg));

    initmsg.sinit_max_instreams = *maxInstreams;
    initmsg.sinit_num_ostreams = *numOstreams;
    printf("Asking for: input streams: %d, output streams: %d\n",
           initmsg.sinit_max_instreams,
           initmsg.sinit_num_ostreams);
    if (setsockopt(server_sockfd, IPPROTO_SCTP,
                   SCTP_INITMSG, &initmsg, sizeof(initmsg))){
        perror("set sock opt\n");
        return 0;
    }

    //specify recive information about streams
    bzero(&events, sizeof(struct sctp_event_subscribe));
    events.sctp_data_io_event = 1;
    setsockopt(server_sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));

    //specify queue
    listen(server_sockfd, 5);

    len = sizeof(struct sockaddr_in);
    *client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &len);
    if(*client_sockfd == -1){
        perror("clien addr");
        return 0;
    }

    memset(&status, 0 , sizeof(status));
    len = sizeof(status);
    if (getsockopt(*client_sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, &len) == -1){
        perror("get sock opt");
        return 0;
    }

    *maxInstreams = status.sstat_instrms;
    *numOstreams  = status.sstat_outstrms;
    printf("Got: input streams: %d, output streams: %d\n\n", *maxInstreams, *numOstreams);

    return 1;
}

int createSocketSend(int *sockfd, int port, char* addr, int *maxInstreams, int *numOstreams){
    struct sockaddr_in* addresses;
    struct sockaddr_in  serv_addr;
    size_t              addr_size = sizeof(struct sockaddr_in);
    struct sctp_status  status;
    size_t              len;
    struct sctp_initmsg initmsg;

    //create endpoint
    *sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    if(*sockfd < 0){
        return 0;
    }

    //create structures to send and receive messages
    addresses = malloc(addr_size);
    if(addresses == NULL){
        return 0;
    }

    //fill parameters for socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    serv_addr.sin_port = htons(port);
    memcpy(addresses, &serv_addr, addr_size);
    bzero(&initmsg, sizeof(struct sctp_initmsg));
    initmsg.sinit_max_instreams = *maxInstreams;
    initmsg.sinit_num_ostreams = *numOstreams;
    printf("Asking %s for: input streams: %d, output streams: %d\n",
           addr,
           initmsg.sinit_max_instreams,
           initmsg.sinit_num_ostreams);

    if (setsockopt(*sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg))){
        free(addresses);
        perror("setsockopt");
        return 0;
    }

    if(sctp_connectx(*sockfd, (struct sockaddr *) addresses, 1) == -1){
        free(addresses);
        perror("connectx");
        return 0;
    }

    memset(&status, 0, sizeof(status));
    len = sizeof(status);
    status.sstat_assoc_id = 1;

    if (getsockopt(*sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, &len) == -1){
        free(addresses);
        perror("getsockopt");
        return 0;
    }
    *numOstreams  = status.sstat_outstrms;
    *maxInstreams = status.sstat_instrms;

    printf("got: input streams: %d, output streams: %d \n",
           *maxInstreams,
           *numOstreams);

    free(addresses);

    return 1;
}


int fileExist (char * fileName){
   FILE * infile;
   int ret = 0;

   infile = fopen (fileName, "r");
   if (infile == NULL){
      ret = 0;
   }else{
      fclose(infile);
      ret = 1;
   }

   return ret;
}

int fsize(FILE *P_File, int mtu, unsigned int *fileSize){

    long pos;
    double num;
    off_t length;

    //read the length
    pos = ftell(P_File); // Guardar el posicion actual del puntero
    fseek(P_File, 0L, SEEK_END); // Ir al final del Fichero
    // Almacenar el Tamaño del Fichero
    length = ftell(P_File);
    *fileSize = (unsigned int)length;
    fseek(P_File, pos, SEEK_SET); // Restablecer puntero
    num = ((float)(*fileSize))/((float)mtu);

    return (int)(ceil(num));
}

int fileSplit(char*** pFileNames, char* srcFileName, int file_mtu){

    char**       fileNames;
    FILE*        pInFileOriginal = NULL; 
    FILE*        pInFilePart = NULL;
    unsigned int totalFileSize;
    int          fileNamesLen;
    int          file_parts;

    char* buffer;
    int   buffer_len = file_mtu; //100MB
    int   read_size;
    int   total_read;
    int   finish_read;

    int i;

    //open the input file
    if((pInFileOriginal = fopen(srcFileName, "r")) == NULL){
        printf("Error opening %s for reading\n", srcFileName);
        return 0;
    }

    //if the file is too large, send it in parts
    file_parts = fsize(pInFileOriginal, file_mtu, &(totalFileSize));
    printf("FILE_MTU: %iMB, TOTAL SIZE: %uMB, FILE_PARTS: %i\n\n", 
           file_mtu/1048576, totalFileSize/1048576, file_parts);

    if(file_parts > 1){

        //buffer space
        if( (buffer = (char*)malloc(buffer_len*sizeof(char))) == NULL){
            printf("Error malloc buffer for file parts");
            return 0;
        }

        //maximun length of the new file names
        fileNamesLen = (strlen(srcFileName)+FILE_EXT_LEN);
        if( (fileNames = (char**)malloc(file_parts*sizeof(char*))) == NULL){
            perror("malloc fileNames");
            return 0;
        }
        *pFileNames = fileNames;
        for(i=0; i<file_parts; i++){
            fileNames[i] = (char*)malloc(fileNamesLen*sizeof(char));
            if(fileNames[i] == NULL){
                return 0;
            }
            sprintf(fileNames[i], "%s.%i", srcFileName, i);
        }

        //for each new file
        for(i=0; i<file_parts; i++){
            
            if(!fileExist(fileNames[i])){
                //open the output file
                if((pInFilePart = fopen(fileNames[i], "w")) == NULL){
                    printf("Error opening %s for reading\n", fileNames[i]);
                    return 0;
                }
            
                finish_read = 0;
                total_read = 0;
                while(!finish_read){
                    //read input and write the output
                    read_size = fread(buffer, sizeof(char), buffer_len, pInFileOriginal);
                    fwrite(buffer, sizeof(char), read_size, pInFilePart);
                    total_read += read_size;
                    if(total_read >= file_mtu || read_size <= 0)
                        finish_read = 1;
                }
                
                //close the output file
                fclose(pInFilePart);
            }
        }

        free(buffer);
    }

    fclose(pInFileOriginal);
    
    return file_parts;
}

void sendStreams(void *thread_inf){
    int         i;
    int         read_size;
    Thread_inf* tinfo = (Thread_inf *)thread_inf;
    int         pos = tinfo->sinfo.sinfo_stream;
    
    //printThreadInf(tinfo);
    for(i=0;i<tinfo->num_msgs; i++){
        
        fseek( tinfo->pFile, pos*(tinfo->mtu), SEEK_SET );
        read_size = fread(tinfo->buffer, sizeof(TRANSFER_UNIT), tinfo->mtu, tinfo->pFile);
        setPos(&tinfo->buffer[(read_size+MTU_DIFF)-1], &tinfo->buffer[(read_size+MTU_DIFF)-2], pos);
        //printf("POS:%i, LEFT:%u, RIGHT:%u\n", pos, tinfo->buffer[(tinfo->mtu)-1], tinfo->buffer[(tinfo->mtu)-2]);
        sctp_send(tinfo->sockfd, tinfo->buffer, sizeof(TRANSFER_UNIT)*(read_size+MTU_DIFF), &(tinfo->sinfo), 0);
        pos = pos+tinfo->parts;
        tinfo->total += read_size;
    }
}


void printThreadInf(Thread_inf *tinfo){
    printf("--THREAD INF--\n");
    printf("sockfd:  %i\n", tinfo->sockfd );
    printf("mtu:     %i\n", tinfo->mtu );
    printf("num_msg: %i\n", tinfo->num_msgs );    
    printf("parts:   %i\n", tinfo->parts );
    printf("total:   %i\n", tinfo->total );
    printf("channel: %i\n", tinfo->sinfo.sinfo_stream );
    printf("-------------------------------\n");
}

int fileReceive(int client_sockfd, int maxChannels){
    struct sockaddr_in     client_addr;
    struct sctp_sndrcvinfo sinfo;
    int                    flags;
    size_t                 len;
    int                    controlChannel = maxChannels-1;
    int                    max_channels =  maxChannels-1;

    FILE*           pOutFile = NULL;
    TRANSFER_UNIT** buffer = NULL;
    TRANSFER_UNIT*  bufferTemp = NULL;
    int*            read_size = NULL;
    int             buffer_len = 0;
    File_inf        file_inf;
    int             readSize = 0;
    int             total = 0;
    int             pos = 0;

    int i;
    int end = 0;

    bzero(&sinfo, sizeof(sinfo));
    bzero(&file_inf, sizeof(File_inf));
    len = sizeof(struct sockaddr_in);


    //receive file information
    sctp_recvmsg(client_sockfd, &file_inf, sizeof(File_inf), (struct sockaddr *) &client_addr, &len, &sinfo, &flags);
    file_inf.name = (char*)malloc(file_inf.name_len*sizeof(char));
    sctp_recvmsg(client_sockfd, file_inf.name, sizeof(char)*file_inf.name_len, (struct sockaddr *) &client_addr, &len, &sinfo, &flags);
    printf("NAME: %s\n", file_inf.name);
    printf("TOTAL SIZE: %uMB\n", file_inf.size/1048576);
    printf("mtu: %iB, parts: %i\n", file_inf.mtu, file_inf.parts);
 
    // Create initial buffers
    buffer_len   = file_inf.parts;
    bufferTemp   = (TRANSFER_UNIT*)malloc((file_inf.mtu+MTU_DIFF)*sizeof(TRANSFER_UNIT));
    read_size    = (int*)malloc(buffer_len*sizeof(int));
    buffer       = (TRANSFER_UNIT**)malloc(buffer_len*sizeof(TRANSFER_UNIT*));
    if(buffer == NULL || bufferTemp == NULL || read_size == NULL){
        printf("initial buffers\n");
        return 1;
    }

    for(i=0; i<buffer_len; i++){
        read_size[i] = 0;
        buffer[i] = (TRANSFER_UNIT*)malloc(file_inf.mtu*sizeof(TRANSFER_UNIT));
        if(buffer[i] == NULL){
            printf("buffer[%i]\n", i);
            return 1;
        }
    }

    len = sizeof(struct sockaddr_in);
    //printf("STATUS: ");
    while(!end){
        readSize = sctp_recvmsg(client_sockfd, bufferTemp, sizeof(TRANSFER_UNIT)*(file_inf.mtu+MTU_DIFF), (struct sockaddr *) &(client_addr), &len, &sinfo, &flags);
        if(sinfo.sinfo_stream == controlChannel && bufferTemp[0] == END){
            end = 1;
        }else{
            pos = getPos(bufferTemp[readSize-1], bufferTemp[readSize-2]);
            //printf("POS:%i, LEFT:%u, RIGHT:%u, READSIZE:%i\n", pos, bufferTemp[readSize-1], bufferTemp[readSize-2], readSize);
            readSize -= MTU_DIFF;
            total += readSize;
            memcpy(buffer[pos], bufferTemp, sizeof(TRANSFER_UNIT)*readSize);
            read_size[pos] = readSize;
            //printf("%1.2f - ",(float)((total*100)/file_inf.size));
        }
    }

    //write the message
    if((pOutFile = fopen(file_inf.name, "w")) == NULL){
        printf("Output file\n");
        return 1;
    }

    for(i=0; i<buffer_len; i++)
        fwrite(buffer[i], sizeof(TRANSFER_UNIT), read_size[i], pOutFile);

    fclose(pOutFile);

    if(buffer != NULL){
        for(i=0; i<buffer_len; i++)
            if(buffer[i] != NULL) free(buffer[i]);
        free(buffer);
    }
    if(bufferTemp != NULL) free(bufferTemp);
    if(read_size != NULL) free(read_size);
    if(file_inf.name != NULL) free(file_inf.name);

    printf("TOTAL: %i\n\n", total);
    return 0;
}



int fileSend(int server_sockfd, int maxChannels, char* srcFileName, int mtu){
    FILE*           pInFileOriginal;
    FILE**          pInFiles = NULL;
    File_inf        file_inf;
    char*           fileName;
    TRANSFER_UNIT** buffer = NULL;
    int             buffer_len = 0;
    int             total = 0;
    int             remainder = 0;
    int             msg_per_buffer = 0;

    struct sctp_sndrcvinfo sinfo;
    struct sockaddr_in*    addresses = NULL;
    int                    controlChannel = maxChannels-1; //the last channel is kept for control
    int                    max_channels =  maxChannels-1;

    pthread_t*              thread = NULL;
    Thread_inf*             thread_inf = NULL;

    int i;
    int value;

    //open the input file
    if((pInFileOriginal = fopen(srcFileName, "r")) == NULL){
        printf("Error opening %s for reading\n", srcFileName);
        return 1;
    }

    //information about the file to transmit
    bzero(&file_inf, sizeof(File_inf));
    //separate filename from the path
    fileName = strrchr(srcFileName, '/')+sizeof(char);
    file_inf.name_len = strlen(fileName)+1;
    file_inf.name = (char*)malloc(file_inf.name_len*sizeof(char));
    if(file_inf.name == NULL){
        return 1;
    }
    strcpy(file_inf.name, fileName);
    file_inf.mtu = mtu;
    file_inf.parts = fsize(pInFileOriginal, file_inf.mtu, &(file_inf.size));
    fclose(pInFileOriginal);
    if(file_inf.parts <= 0){
        printf("fsize problem. Parts:%i FileSize:%i \n", file_inf.parts, file_inf.size);
        return 1;
    }else{
        if( file_inf.parts >  MAX_NUM_PARTS){
            printf("Error: number maximum of parts. Use a bigger MTU\n");            
            return 0;
        }
    }

    printf("NAME: %s\n", file_inf.name);
    printf("TOTAL SIZE: %uMB\n", file_inf.size/1048576);
    printf("mtu: %iB, parts: %i\n", file_inf.mtu, file_inf.parts);
   
    //Init channel
    bzero(&sinfo, sizeof(sinfo));
    sinfo.sinfo_stream = controlChannel;

    //send file_inf
    sctp_send(server_sockfd, &file_inf, sizeof(File_inf), &sinfo, 0);
    sctp_send(server_sockfd, file_inf.name, sizeof(char)*file_inf.name_len, &sinfo, 0);

    //Calcule parts
    if(file_inf.parts > max_channels){
        remainder      = file_inf.parts%max_channels;
        msg_per_buffer = file_inf.parts/max_channels;
        buffer_len     = max_channels;
    }else{
        msg_per_buffer = 1;
        buffer_len = file_inf.parts;
    }

    // Create initial buffers
    buffer = (TRANSFER_UNIT**)malloc(sizeof(TRANSFER_UNIT*)*buffer_len);
    if(buffer == NULL){
        return 1;
    }

    for(i=0; i<buffer_len; i++){
        buffer[i] = (TRANSFER_UNIT*)malloc((file_inf.mtu+MTU_DIFF)*sizeof(TRANSFER_UNIT));
        if(buffer[i] == NULL){
            return 1;
        }
    }

    // Open the file to be read by the threads
    pInFiles = (FILE**)malloc(sizeof(FILE*)*buffer_len);
    if(pInFiles == NULL){
        return 1;
    }
 
    //create threads to send messages
    thread = malloc(sizeof(pthread_t)*buffer_len);
    thread_inf = malloc(sizeof(Thread_inf)*buffer_len);
    //fill arguments in the thread
    for(i=0;i<buffer_len; i++){        
        bzero(&(thread_inf[i].sinfo), sizeof(sinfo));
        //open the input file
        if((pInFiles[i] = fopen(srcFileName, "r")) == NULL){
            printf("Error opening %s for reading in loop %i\n", srcFileName, i);
            return 1;
        }

        thread_inf[i].sinfo.sinfo_stream = i;
        thread_inf[i].sockfd             = server_sockfd;
        thread_inf[i].buffer             = buffer[i];
        thread_inf[i].mtu                = file_inf.mtu;
        thread_inf[i].pFile              = pInFiles[i];
        thread_inf[i].parts              = buffer_len;
        thread_inf[i].total              = 0;
        if(remainder > 0){
            thread_inf[i].num_msgs = msg_per_buffer+1;
            remainder--;
        }else{
            thread_inf[i].num_msgs = msg_per_buffer;
        }
            
        //create thread
        if(pthread_create(&thread[i], NULL, sendStreams, (void *)&thread_inf[i])){
            perror("pthread\n");
            return 1;
        }
    }
        
    //wait for all the threads end
    //printf("STATUS: ");
    for(i=0; i<buffer_len;i++){
        pthread_join( thread[i], NULL);
        total += thread_inf[i].total;
        //printf("%1.2f - ",(float)((total*100)/file_inf.size));
    }

    value = END;
    sctp_send(server_sockfd, &value, sizeof(int), &(sinfo), 0);

    if(buffer != NULL){
        for(i = 0; i<buffer_len; i++)
            if(buffer[i] != NULL) free(buffer[i]);
        free(buffer);
    }
    if(pInFiles != NULL){
        for(i = 0; i<buffer_len; i++)
            if(pInFiles[i] != NULL) fclose(pInFiles[i]);
        free(pInFiles);
    }
    if(thread != NULL) free(thread);
    if(thread_inf!= NULL) free(thread_inf);
    if(addresses != NULL) free(addresses);
    if(file_inf.name != NULL) free(file_inf.name);

    printf("TOTAL: %i\n\n", total);
    return 0;
}

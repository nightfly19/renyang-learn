#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "serialsource.h"

int ProcessImageFromEthernetToPC (int* fd_lcd);

extern int globle_flag=2;

static char *msgs[] = {
  "unknown_packet_type",
  "ack_timeout"	,
  "sync"	,
  "too_long"	,
  "too_short"	,
  "bad_sync"	,
  "bad_crc"	,
  "closed"	,
  "no_memory"	,
  "unix_error"
};

void stderr_msg(serial_source_msg problem)
{
  fprintf(stderr, "Note: %s\n", msgs[problem]);
}

int main(int argc, char **argv)
{
  serial_source src;
  int p16, p18, p20, q16, q18, q20, flagT=1, running=0;

  pthread_t camera;
  pthread_t voice;

  if (argc != 3)
    {
      fprintf(stderr, "Usage: %s <device> <rate> - dump packets from a serial port\n", argv[0]);
      exit(2);
    }
  // 開啟sensor
  src = open_serial_source(argv[1], platform_baud_rate(argv[2]), 0, stderr_msg);
  if (!src)
    {
      fprintf(stderr, "Couldn't open serial port at %s:%s\n",
	      argv[1], argv[2]);
      exit(1);
    }

  for (;;)
    {
      int len, i;
      const unsigned char *packet = read_serial_packet(src, &len);
      if (!packet)
	  exit(0);

      /*for (i = 0; i < len; i++)
		printf("%02x ", packet[i]);
      putchar('\n');*/

	if(packet[14] != 99) {
		p16 = packet[16];
      	p18 = packet[18];
      	p20 = packet[20];
	}
	
	if(packet[14] == 99) {
		if(globle_flag == 2)
			globle_flag = 0;
		else if(globle_flag ==0) {
			globle_flag = 1;
			break;
		}
	}


      if( ((q16 != p16) || (q18 != p18) || (q20 != p20)) && (flagT>=2) && (running==0) && (globle_flag != 2)) 
        {    printf("Moved at pkt%02x !!! \n", packet[15]);
             flagT=0;  
		 running = 1;
             //ADD your function here for IODD project !!!! START
		 system("/proj/voice/alarm --no-tty-control -r /proj/voice/alarm.mp3&");
		 //pthread_create(&voice, NULL, system, (void*) "./voice/alarm -r ./voice/alarm.mp3");
 		 pthread_create(&camera, NULL, ProcessImageFromEthernetToPC, (void*) 0);
		 //ProcessImageFromEthernetToPC(0, 0);
             //ADD your function here for IODD project !!!! END

        }
      

      q16 = p16;
      q18 = p18;
      q20 = p20;

      flagT+=1;


      free((void *)packet);
    }
    system("killall -9 alarm");
}

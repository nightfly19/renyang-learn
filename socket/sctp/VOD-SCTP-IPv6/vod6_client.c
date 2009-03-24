/* Written by J. S. Ha (mugal1@cs.knu.ac.kr), October 2005 */

#include <math.h>
#include <sys/time.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <xine.h>
#include <xine/xineutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/sctp.h>


#define BUFSIZE 1024*4
#define NAMESIZE 20


/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VERSION
#define VERSION                   "0.0.6"
#endif


static xine_t              *xine;
static xine_stream_t       *stream;
static xine_video_port_t   *vo_port;
static xine_audio_port_t   *ao_port;
static xine_event_queue_t  *event_queue;

static Display             *display;
static int                  screen;
static Window               window;
static int                  xpos, ypos, width, height;
static double               pixel_aspect;
static int                  running = 0;

static int readsize=0;

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)

static void dest_size_cb(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect)  {

  *dest_width        = width;
  *dest_height       = height;
  *dest_pixel_aspect = pixel_aspect;
}

static void frame_output_cb(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y) {
  *dest_x            = 0;
  *dest_y            = 0;
  *win_x             = xpos;
  *win_y             = ypos;
  *dest_width        = width;
  *dest_height       = height;
  *dest_pixel_aspect = pixel_aspect;
}

static void event_listener(void *user_data, const xine_event_t *event) {
  switch(event->type) { 
  case XINE_EVENT_UI_PLAYBACK_FINISHED:
    running = 0;
    break;
  }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////

void * xine_start(void * arg);
void * send_message(void *arg);
void * recv_message(void *arg);
void error_handling(char *message);

char name[NAMESIZE]="[Default]";
char message[BUFSIZE];
char vod_list[BUFSIZE];


int fdout;
int flags;
int mode;
int outbuf[BUFSIZE];

int streaming_flag=0;

pthread_t xine_thread;
void * thread_result;
int move;



int main(int argc, char **argv)
{
  int sock;
  void sigmsg(void);//시그널 포획함수
  struct sockaddr_in6 serv_addr;
  pthread_t snd_thread, rcv_thread;
  char   ipv6_addr[16];
  char    addr6_str[40];

  
  if(argc!=3){
    printf("Usage : %s <IP> <port> \n", argv[0]);
    exit(1);
  }
  
  if(signal(SIGINT,(void *)sigmsg)==SIG_ERR)
  {
	perror("signal");
	exit(1);
  }



  mode=0600;
  flags=O_CREAT|O_WRONLY|O_TRUNC;

  if((fdout=open("temp.avi",flags, mode))==-1)
  {
		perror("open");
		exit(1);
  }

/////////////////////////////////////////////
//  sock=socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
//  if(sock==-1)
//	  error_handling("socket() error");

//  memset(&serv_addr, 0, sizeof(serv_addr));
//  serv_addr.sin_family=AF_INET;
//  serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
//  serv_addr.sin_port=htons(atoi(argv[2]));
  //////////////////////////////////////////////////
  sock = socket(AF_INET6, SOCK_STREAM,IPPROTO_SCTP);
  if (sock < 0)
  {
        perror("socket create error:");
        exit(0);
  }

    serv_addr.sin6_family   = AF_INET6;
    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_port     = htons(atoi(argv[2]));

    inet_pton(AF_INET6, argv[1], (void *)&ipv6_addr);
    // in6addr_loopback 는 loopback 주소로 연결하기 위해서 
    // 사용한다.  
    // ipv6 에서의 loopbak 주소는 "::1" 이다.  
    // 루프백주소로 연결하길 원한다면
    // svrsin6.sin6_addr      = in6addr_loopback;   
    // 혹은
    // inet_pton(AF_INET6, "::1", (void *)&ipv6_addr); 
    // 하면된다. 
    memcpy((void *)&serv_addr.sin6_addr, (void *)&ipv6_addr, 16);
    inet_ntop(AF_INET6, (void *)&serv_addr.sin6_addr, addr6_str, 40);

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("connect() error");


  printf("SCTP VOD서버에 접속되었습니다\n");
  pthread_create(&snd_thread, NULL, send_message, (void*)sock);
  pthread_create(&rcv_thread, NULL, recv_message, (void*)sock);

  pthread_join(snd_thread, &thread_result);
  pthread_join(rcv_thread, &thread_result);

  close(sock);  
  return 0;
}

void * send_message(void *arg) /* 메시지 전송 쓰레드 실행 함수 */
{
   int sock = (int)arg;
   char name_message[NAMESIZE+BUFSIZE];
   while(1) {
      fgets(message, BUFSIZE, stdin);
	  if(!strcmp(message,"q\n")) {  /* 'q' 입력 시 종료 */
	     
         close(sock);
		 unlink("temp.avi");
         exit(0);
      }
	  
	  if(!strncmp(message,"##",2)&&streaming_flag==0) 
	  {  /* 원하는 vod 파일 요구 */
	     printf("파일 전송을 요청합니다.\n");
         write(sock,message, strlen(message));
		 streaming_flag=1;
		 move=1;

		 sleep(5);
		 
		 pthread_create(&xine_thread,NULL, xine_start,(void*)sock);    
         pthread_join(xine_thread, &thread_result);
      }
	  else
	  {
		 printf("스트리밍 중이거나 파일 요청 포맷이 일치 하지 않습니다\n");
	  }
  
   }
}

void * recv_message(void *arg) /* 메시지 수신 쓰레드 실행 함수 */
{
  int sock = (int)arg;
  int str_len;


  while(1)
  {
    str_len = read(sock, message, BUFSIZE);
	if(str_len==-1) return 1;

	//printf("%s",message);
    if(!strncmp(message,"##play-list##",13)) 
	{  
		printf("**********************VOD-list**********************\n");
		printf("%s",message+13);
		printf("****************************************************\n");
		printf("****위의 목록중에서 원하는 contents를 선택하세요****\n");
		printf("ex)##1\n\n\n\n\n\n\n");

		sprintf(vod_list, "%s", message+13);
		
    }
    else if(!strncmp(message,"end",3))
	{
		printf("서비스 전송을 완료 하였습니다.\n\n\n\n");

		printf("**********************VOD-list**********************\n");
		printf("%s",vod_list);
		printf("****************************************************\n");
		printf("****위의 목록중에서 원하는 contents를 선택하세요****\n");
		printf("ex)##1\n\n\n\n\n\n");
		streaming_flag=0;
		
	}
	else if(!strncmp(message,"***start",8))
	{
		printf("서비스를 위해 데이터 받기를 시작합니다.\n");
		printf("xine-thread 생성...................\n");


	}
	else
	{
		write(fdout, message, str_len);


	}
	memset(message,'0',BUFSIZE);
  }
}


void *xine_start(void * arg)
{
  int sock = (int)arg;
  char              configfile[2048];
  x11_visual_t      vis;
  double            res_h, res_v;
  char             *vo_driver    = "auto";
  char             *ao_driver    = "auto";
  char             *mrl = NULL;
  int               i;

printf("muxine (xine 占쏙옙占쏙옙占쏙옙??????占쏙옙??占?r) v%s\n"
	 "(c) 2003 by Daniel Caujolle-Bert <f1rmb@users.sourceforge.net>.\n", VERSION);

//sleep(10000);
	 /*
  if(argc <= 1) {
    printf("specify one mrl\n");
    return 0;
  }
  
  if(argc > 1) {
    for(i = 1; i < argc; i++) {
      if(!strcmp(argv[i], "-vo")) {
	vo_driver = argv[++i];
      }
      else if(!strcmp(argv[i], "-ao")) {
	ao_driver = argv[++i];
      }
      else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {
	printf("Options:\n");
	printf("  -ao <ao name>           Audio output plugin name (default = alsa).\n");
	printf("  -vo <vo name>           Video output plugin name (default = Xv).\n");
	return 0;
      }      
      else {
	mrl = argv[i];
      }
    }
  }
  else
    mrl = argv[1];
*/

 mrl="temp.avi";
  
  if(!XInitThreads()) {
    printf("XInitThreads() failed\n");
    return 0;
  }

  xine = xine_new();
  sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
  xine_config_load(xine, configfile);
  xine_init(xine);
 
  if((display = XOpenDisplay(getenv("DISPLAY"))) == NULL) {
    printf("XOpenDisplay() failed.\n");
    return 0;
  }
    
  screen       = XDefaultScreen(display);
  xpos         = 0;
  ypos         = 0;
  width        = 320;
  height       = 200;

  XLockDisplay(display);
  window = XCreateSimpleWindow(display, XDefaultRootWindow(display),
			       xpos, ypos, width, height, 1, 0, 0);
  
  XSelectInput (display, window, INPUT_MOTION);

  XMapRaised(display, window);
  
  res_h = (DisplayWidth(display, screen) * 1000 / DisplayWidthMM(display, screen));
  res_v = (DisplayHeight(display, screen) * 1000 / DisplayHeightMM(display, screen));
  XSync(display, False);
  XUnlockDisplay(display);

  vis.display           = display;
  vis.screen            = screen;
  vis.d                 = window;
  vis.dest_size_cb      = dest_size_cb;
  vis.frame_output_cb   = frame_output_cb;
  vis.user_data         = NULL;
  pixel_aspect          = res_v / res_h;

  if(fabs(pixel_aspect - 1.0) < 0.01)
    pixel_aspect = 1.0;
  
  if((vo_port = xine_open_video_driver(xine, 
				       vo_driver, XINE_VISUAL_TYPE_X11, (void *) &vis)) == NULL) {
    printf("I'm unable to initialize '%s' video driver. Giving up.\n", vo_driver);
    return 0;
  }
  
  ao_port     = xine_open_audio_driver(xine , ao_driver, NULL);
  stream      = xine_stream_new(xine, ao_port, vo_port);
  event_queue = xine_event_new_queue(stream);
  xine_event_create_listener_thread(event_queue, event_listener, NULL);

  xine_gui_send_vo_data(stream, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) window);
  xine_gui_send_vo_data(stream, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);

  if((!xine_open(stream, mrl)) || (!xine_play(stream, 0, 0))) {
    printf("Unable to open mrl '%s'\n", mrl);
    return 0;
  }

  running = 1;

  while(running) {
    XEvent xevent;
    int    got_event;

    XLockDisplay(display);
    got_event = XCheckMaskEvent(display, INPUT_MOTION, &xevent);
    XUnlockDisplay(display);
    
    if( !got_event ) {
      xine_usec_sleep(20000);
      continue;
    }

    switch(xevent.type) {

 printf("sdfsdf\n");
    case Expose:
      if(xevent.xexpose.count != 0)
        break;
      xine_gui_send_vo_data(stream, XINE_GUI_SEND_EXPOSE_EVENT, &xevent);
      break;
      
	case DestroyNotify:
		{
		  printf("sdfsdf\n");
		  break;
		}
      

    case ConfigureNotify:
      {
	XConfigureEvent *cev = (XConfigureEvent *) &xevent;
	Window           tmp_win;
	
	width  = cev->width;
	height = cev->height;
	
	if((cev->x == 0) && (cev->y == 0)) {
	  XLockDisplay(display);
	  XTranslateCoordinates(display, cev->window,
				DefaultRootWindow(cev->display),
				0, 0, &xpos, &ypos, &tmp_win);
	  XUnlockDisplay(display);
	}
	else {
	  xpos = cev->x;
	  ypos = cev->y;
	}
      }
      break;

    }
  }
  
  close(sock);
  printf("종료 되었습니다\n");
  xine_close(stream);
  xine_event_dispose_queue(event_queue);
  xine_dispose(stream);
  if(ao_port)
    xine_close_audio_driver(xine, ao_port);  
  xine_close_video_driver(xine, vo_port);  
  xine_exit(xine);
  
  XLockDisplay(display);
  XUnmapWindow(display,  window);
  XDestroyWindow(display, window);
  XUnlockDisplay(display);
  
  XCloseDisplay (display);
}


void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}


void sigmsg(void)
{
		printf("종료를 원하시면 키보드에서 q 키를 누르세요\n");
		if(signal(SIGINT, (void *)sigmsg)==SIG_ERR)
	    {
			perror("signal");

	    }
}
//gcc -Wall -O2 `xine-config --cflags` `xine-config --libs` -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm -o vod_client vod_client.c

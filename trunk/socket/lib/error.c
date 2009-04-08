#include "unp.h"

#include <stdarg.h>	// ANSI C header file
#include <syslog.h>	// for syslog()

int daemon_proc;	// set nonzero by daemon_init()

static void err_doit(int,int,const char *,va_list);

void err_ret(const char *fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,LOG_INFO,fmt,ap);
	va_end(ap);
	return ;
}

void err_quit(const char *fmt, ...)
{
	va_list ap;	// 一個特殊的型態(type),在va_start,va_arg,va_end三個巨集中使用

	va_start(ap,fmt);	// 啟始不定長度引數的巨集
	err_doit(0,LOG_ERR,fmt,ap);
	va_end(ap);		// 終止不定長度引數的巨集
	exit(1);
}

void err_sys(const char *fmt, ...)
{
	va_list ap;
	
	va_start(ap,fmt);
	err_doit(1,LOG_ERR,fmt,ap);
	va_end(ap);
	exit(1);
}

static void err_doit(int errnoflag,int level,const char *fmt,va_list ap)
{
	int errno_save,n;
	char buf[MAXLINE+1];

	errno_save = errno;	// value caller might want printed
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf,MAXLINE,fmt,ap);
#else
	vsprintf(buf,fmt,ap);
#endif
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf + n,MAXLINE - n,": %s",strerror(errno_save));
	strcat(buf,"\n");

	if (daemon_proc) {
		syslog(level,buf);
	} else {
		fflush(stdout);	// in case stdout and stderr are the same
		fputs(buf,stderr);
		fflush(stderr);
	}
	return ;
}

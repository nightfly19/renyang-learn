/***************************** setcolor.h *************************/
/* This is a Unix API for changing the color in console
******************************************************************/
#ifndef _SETCOLOR_H
#define _SETCOLOR_H

#include <stdio.h>

#define BLACK		"\033[22;30m"
#define RED			"\033[22;31m"
#define GREEN		"\033[22;32m"
#define YELLOW		"\033[22;33m"
#define BLUE		"\033[22;34m"
#define MAGENTA	"\033[22;35m"
#define CYAN		"\033[22;36m"
#define WHITE		"\033[22;37m"
#define BGBALCK	"\033[22;40m"
#define BGRED		"\033[22;41m"
#define BGGREEN	"\033[22;42m"
#define BGYELLOW	"\033[22;43m"
#define BGBLUE		"\033[22;44m"
#define BGMAGENTA	"\033[22;45m"
#define BGCYAN		"\033[22;46m"
#define BGWHITE	"\033[22;47m"
#define BGDEFAULT	"\033[22;49m"
#define BBLACK		"\033[1;30m"
#define BRED		"\033[1;31m"
#define BGREEN		"\033[1;32m"
#define BYELLOW	"\033[1;33m"
#define BBLUE		"\033[1;34m"
#define BMAGENTA	"\033[1;35m"
#define BCYAN		"\033[1;36m"
#define BWHITE		"\033[1;37m"

static void setColor(const char* color);
static void setColor(const char* color)
{
	printf("%s",color);
}

#endif

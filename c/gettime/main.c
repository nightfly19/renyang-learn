#include <stdio.h>
#include <string.h>
#include <time.h>

int main( void )
{
        struct tm *newtime;
        char am_pm[] = "AM";
        time_t long_time;

        time( &long_time );                /* Get time as long integer. */
        newtime = localtime( &long_time ); /* Convert to local time. */

        if( newtime->tm_hour > 12 )        /* Set up extension. */
                strcpy( am_pm, "PM" );
        if( newtime->tm_hour > 12 )        /* Convert from 24-hour */
                newtime->tm_hour -= 12;    /*   to 12-hour clock.  */
        if( newtime->tm_hour == 0 )        /*Set hour to 12 if midnight. */
                newtime->tm_hour = 12;

        printf( "%.19s %s\n", asctime( newtime ), am_pm );
}


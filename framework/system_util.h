/*
 * system_util.h
 *
 *  Created on: 2011-10-14
 *      Author: Administrator
 */

#ifndef SYSTEM_UTIL_H_
#define SYSTEM_UTIL_H_


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>


namespace kingnet
{

#define error_exit(num,fmt,args...)  \
    do{fprintf(stderr,"%ld,%s:%d,%d:%s," fmt "\n",time(NULL),__FILE__,__LINE__,errno,strerror(errno),##args);exit(num);} while(0)

#define error_return(num,fmt,args...)  \
    do{fprintf(stderr,"%ld,%s:%d,%d:%s," fmt "\n",time(NULL),__FILE__,__LINE__,errno,strerror(errno),##args);return(num);} while(0)

#ifdef NDEBUG
#define debug_format(fmt,args...)

#else
#define debug_format(fmt,args...)  \
    do{fprintf(stdout,"%ld,%s:%d," fmt "\n",time(NULL),__FILE__,__LINE__,##args);}while(0)

#endif

int set_open_file_limit(int maxsize) ;

int get_open_file_limit() ;

int lock_file(const char* filename) ;

void set_process_title(int argc,char* argv[],const char* fmt, ...) ;

int  set_thread_title(const char* fmt, ...) ;

int daemon_init(int nochdir,int noclose) ;
}

#endif /* SYSTEM_UTIL_H_ */

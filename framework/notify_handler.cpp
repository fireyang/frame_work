/*
 * notify_handler.cpp
 *
 *  Created on: 2011-10-22
 *      Author: lxyfirst@yahoo.com.cn
 */
#include <unistd.h>
#include <errno.h>
#include "network_util.h"
#include "notify_handler.h"


namespace kingnet
{



void notify_handler::on_read(int fd)
{

    char buf[16] ;
    int len = read(fd,buf,sizeof(buf)) ;
    if(len >0)
    {
        on_notify() ;
    }
    else if(len <0)
    {
        if (errno != EAGAIN &&  errno != EINTR)  on_error(fd)  ;
    }
    else
    {
        on_error(fd)  ;
    }

}

void notify_handler::on_write(int fd)
{

}

void notify_handler::on_error(int fd)
{
    close(m_fd) ;
}



int notify_handler::send_notify()
{
    static const char buf[1] = {'v'} ;
    return write(m_fd,buf,1);
}




}


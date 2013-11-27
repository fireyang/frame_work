/*
 * tcp_acceptor.cpp
 *
 *  Created on: 2011-10-31
 *      Author: Administrator
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "tcp_acceptor.h"

namespace kingnet
{

tcp_acceptor::tcp_acceptor():m_reactor(NULL),m_fd(-1)
{
    // TODO Auto-generated constructor stub

}

tcp_acceptor::~tcp_acceptor()
{
    fini() ;
}

int tcp_acceptor::init(epoll_reactor* reactor,const char* host,int port)
{
    if(m_fd >= 0 || reactor == NULL || host == NULL ) return -1 ;
    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;
    int sfd = create_tcp_service(&service_addr) ;
    if(sfd <0 ) return -2 ;

    if( reactor->add_handler(sfd,this,epoll_reactor::EVENT_READ)!=0 )
    {
        close(sfd) ;
        return -3 ;
    }
    m_reactor = reactor ;
    m_fd = sfd ;

    return 0 ;

}

void tcp_acceptor::fini()
{
    if(m_fd >= 0 )
    {
        m_reactor->del_handler(m_fd) ;
        close(m_fd) ;
        m_fd = -1 ;

    }
}

void tcp_acceptor::on_read(int fd)
{


    socklen_t addr_len;
    sa_in_t caddr = {0};
    addr_len = sizeof(caddr);

    int cfd = 0 ;
    for(int i = 0 ; i< MAX_ACCEPT_ONCE ; ++i )
    {
        cfd = accept(m_fd,(sa_t*)&caddr,&addr_len) ;
        if(cfd >= 0 )
        {
            if ( on_connection(cfd,&caddr) != 0 ) close(cfd) ;
        }
        else
        {
            if(errno == EAGAIN || errno ==EMFILE || errno ==ENFILE) break ;
            else if ( errno == EINTR || errno ==ECONNABORTED ) continue ;
            else on_error(m_fd) ;
        }
    }


}

void tcp_acceptor::on_write(int fd)
{

}

void tcp_acceptor::on_error(int fd)
{
    fini() ;
}

}

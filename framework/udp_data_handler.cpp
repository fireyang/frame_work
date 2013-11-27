/*******************************************************************************
 *
 *      @file: udp_data_handler.cpp
 *      @brief: handle read/write udp data from/to network.
 *
 *      @author: gxl.
 *      @Date: 2012/10/08
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "time_util.h"
#include "udp_data_handler.h"


namespace kingnet
{

udp_data_handler::udp_data_handler()
    : sock_fd(-1),
    m_port(0),
    m_reactor(NULL)
{
    // TODO Auto-generated constructor stub
}

udp_data_handler::~udp_data_handler()
{
    fini() ;
}

int udp_data_handler::init(epoll_reactor* reactor,const char* host,int port)
{
    if( host == NULL ) return -1 ;
    
    m_port = port;

    init_sa_in(&service_addr, host, port) ;

    sock_fd = create_udp_client(&service_addr, 0) ;
    if(sock_fd < 0 ) return -2 ;

    int ret = init(reactor, sock_fd) ;
    if( ret != 0 )
    {
        close(sock_fd) ;
    }

    return ret ;
}

int udp_data_handler::init(epoll_reactor* reactor,int fd)
{
    if(fd < 0 || reactor == NULL ) return -1 ;

    m_rbuf.init(INIT_RECV_BUF_SIZE) ;
    m_sbuf.init(INIT_SEND_BUF_SIZE) ;
    if(m_rbuf.space() == NULL || m_sbuf.space() == NULL )
    {
        return -2 ;
    }

    if( reactor->add_handler(fd,this,epoll_reactor::EVENT_READ)!=0 )
    {
        return -3 ;
    }

    m_reactor = reactor ;
    sock_fd = fd ;

    return 0 ;
}


void udp_data_handler::fini()
{
    if(sock_fd >= 0 )
    {
        //on_disconnect() ;
        m_reactor->del_handler(sock_fd) ;
        close(sock_fd) ;
        sock_fd = -1 ;
        m_reactor = NULL ;
        m_rbuf.fini() ;
        m_sbuf.fini() ;
    }
}

void udp_data_handler::on_read(int fd)
{
    int recv_len = read(fd,m_rbuf.space(),m_rbuf.space_size()) ;

    if(recv_len > 0)
    {
        m_rbuf.push_data(recv_len) ;
        while(m_rbuf.data_size() > 0)
        {
            packet_info pi = {0} ;
            int ret = 0 ;
            if((ret=get_packet_info(m_rbuf.data(),m_rbuf.data_size(),&pi))!=0)
            {
                handle_error(ret);
                return ;
            }
                 
            if( pi.size < 1 || pi.size > 4194304 )
            {
                handle_error(ERROR_TYPE_REQUEST) ;
                return ;
            }

            if(m_rbuf.data_size() >= pi.size )
            {
                if((ret=process_packet(&pi)) !=0 )
                {
                    handle_error(ret) ;
                    return ;
                }

                m_rbuf.pop_data(pi.size) ;
            }
            else
            {
                if(m_rbuf.space_size() < pi.size - m_rbuf.data_size())
                {
                    if(m_rbuf.resize(m_rbuf.capacity() + pi.size )!=0)
                    {
                        handle_error(ERROR_TYPE_MEMORY) ;
                        return ;
                    }
                }

                break ;
            }

        }

        m_rbuf.adjust() ;
    }
    else if (errno != EAGAIN &&  errno != EINTR)
    {
        handle_error(ERROR_TYPE_SYSTEM)  ;
    }


}

void udp_data_handler::on_write(int fd)
{
    if(m_sbuf.data_size() > 0 )
    {
        int send_size = write(fd, m_sbuf.data(), m_sbuf.data_size() );
        if(send_size >0)
        {
            m_sbuf.pop_data(send_size) ;
            m_sbuf.adjust() ;
        }
        else if ( send_size < 0)
        {
            return ;
            /*
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM) ;
                return ;
            }
            */
        }
    }

    if(m_sbuf.data_size() == 0)
    {
        m_reactor->mod_handler(fd,this,epoll_reactor::EVENT_READ) ;

    }

}

void udp_data_handler::on_error(int fd)
{
    handle_error(ERROR_TYPE_SYSTEM) ;
}

int udp_data_handler::send(const char* data,int size,int delay_flag)
{
    int send_size = 0 ;
    //try send data directly
    if(m_sbuf.data_size()==0 && delay_flag ==0)
    {
		send_size = write(sock_fd, m_sbuf.data(), m_sbuf.data_size());
        
        if(send_size <0)
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                return -1 ;
            }
            send_size = 0 ;
        }
        if(send_size == size ) return 0 ;
    }

    //push remaining data to send buffer
    size -= send_size ;
    if(m_sbuf.space_size() < size &&(m_sbuf.resize(m_sbuf.capacity() + size )!=0) )
    {
        return -1 ;
    }

    memcpy(m_sbuf.space(),data + send_size , size) ;
    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(sock_fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    if(delay_flag == 0) on_write(sock_fd) ;

    return 0 ;
}

int udp_data_handler::send( packet *p,int delay_flag)
{
    int size = p->encode_size() ;
    if(m_sbuf.space_size() < size && m_sbuf.resize(m_sbuf.capacity()+ size )!=0)
    {
        return -1 ;
    }

    size = p->encode(m_sbuf.space(),m_sbuf.space_size()) ;
    if ( size < 0 ) return -1 ;
    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(sock_fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    if(delay_flag == 0) on_write(sock_fd) ;

    return 0 ;
}

void udp_data_handler::handle_error(int error_type)
{
    on_event(error_type) ;

}

int udp_data_handler::get_sock_addr(sa_in_t* addr) const
{
    if(sock_fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getsockname(sock_fd,(struct sockaddr*)addr,&addr_size) ;

}

int udp_data_handler::get_remote_addr(sa_in_t* addr) const
{
    if(sock_fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getpeername(sock_fd,(struct sockaddr*)addr,&addr_size) ;
}




}

/*
 * tcp_connection.cpp
 *
 *  Created on: 2011-11-2
 *      Author: lxyfirst@yahoo.com.cn
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
//#include <stdio.h>

#include "time_util.h"
#include "tcp_connection.h"
#include "packet.h"
#include "packet_processor.h"

namespace kingnet
{

tcp_connection::tcp_connection():m_reactor(NULL),m_processor(NULL)
{
    // TODO Auto-generated constructor stub

    m_id.fd = -1 ;
    m_id.timestamp = 0 ;
}

tcp_connection::~tcp_connection()
{
    fini() ;
}

int tcp_connection::init(epoll_reactor* reactor,const char* host,int port,packet_processor *processor)
{
    if(m_id.fd >= 0 || host == NULL ) return -1 ;
    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;

    int fd = create_tcp_client(&service_addr,0) ;
    if(fd < 0 ) return -2 ;

    int ret = init(reactor,fd,processor) ;
    if( ret != 0 )
    {
        close(fd) ;
    }

    return ret ;
}

int tcp_connection::init(epoll_reactor* reactor,int fd,packet_processor *processor)
{
    if(m_id.fd >= 0 || fd < 0 || reactor == NULL || processor == NULL) return -1 ;
    set_nonblock(fd) ;
    m_rbuf.init(1024) ;
    m_sbuf.init(1024) ;
    if(m_rbuf.space() == NULL || m_sbuf.space() == NULL )
    {
        return -2 ;
    }

    if( reactor->add_handler(fd,this,epoll_reactor::EVENT_READ)!=0 )
    {
        return -3 ;
    }

    m_reactor = reactor ;
    m_processor = processor ;
    m_id.fd = fd ;
    m_id.timestamp = time(0) ;

    processor->on_connected(this) ;
    return 0 ;
}


void tcp_connection::fini()
{
    if(m_id.fd >= 0 )
    {
        m_processor->on_disconnect(this) ;
        m_reactor->del_handler(m_id.fd) ;
        close(m_id.fd) ;
        m_id.fd = -1 ;
        m_id.timestamp = 0 ;
        m_reactor = NULL ;
        m_rbuf.fini() ;
        m_sbuf.fini() ;
    }
}

void tcp_connection::on_read(int fd)
{

    int recv_len = read(fd,m_rbuf.space(),m_rbuf.space_size()) ;

    if(recv_len >0)
    {
        m_rbuf.push_data(recv_len) ;
        while(m_rbuf.data_size() > 0)
        {
            packet_info pi ;

            if(m_processor->get_info(m_rbuf.data(),m_rbuf.data_size(),&pi)!=0 || 
                pi.size < 1 )
            {
                handle_error(ERROR_TYPE_REQUEST) ;
                return ;
            }

            if(m_rbuf.data_size() >= pi.size )
            {

                if(m_processor->process(this,&pi) !=0 )
                {
                    handle_error(ERROR_TYPE_REQUEST) ;
                    return ;
                }


                m_rbuf.pop_data(pi.size) ;
            }
            else
            {
                if(m_rbuf.space_size() < pi.size - m_rbuf.data_size() &&
                    m_rbuf.resize(m_rbuf.capacity() + pi.size )!=0 )
                {
                    handle_error(ERROR_TYPE_MEMORY) ;
                    return ;
                }

                break ;
            }
        }

        m_rbuf.adjust() ;
    }
    else if(recv_len == 0)
    {
        //peer close
        handle_error(ERROR_TYPE_PEER_CLOSE) ;
        
    }
    else if (errno != EAGAIN ||  errno != EINTR)
    {
        handle_error(ERROR_TYPE_SYSTEM)  ;
    }


}

void tcp_connection::on_write(int fd)
{
    if(m_sbuf.data_size() > 0 )
    {
        int send_len = write(fd,m_sbuf.data(),m_sbuf.data_size()) ;
        if(send_len >0)
        {
            m_sbuf.pop_data(send_len) ;
            m_sbuf.adjust() ;

        }
        else if ( send_len < 0)
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM) ;
                return ;
            }
        }
    }

    if(m_sbuf.data_size() == 0)
    {
        m_reactor->mod_handler(fd,this,epoll_reactor::EVENT_READ) ;

    }

}

void tcp_connection::on_error(int fd)
{
    handle_error(ERROR_TYPE_SYSTEM) ;

    /*
    int error_code = get_socket_error(fd) ;
    if ( error_code !=0 &&  error_code != EAGAIN && error_code != EINTR )
    {
        errno = error_code ;
        handle_error(ERROR_TYPE_SYSTEM) ;

    }
    */

}

int tcp_connection::send(const char* data,int size,int delay_flag)
{
    int send_len = 0 ;
    //try send data directly
    if(m_sbuf.data_size()==0  && delay_flag ==0)
    {
        send_len = write(m_id.fd,data,size) ;
        if(send_len <0)
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                return -1 ;
            }
            send_len = 0 ;
        }

        if(send_len == size ) return 0 ;
    }

    //push remaining data to send buffer
    size -= send_len ;
    if(m_sbuf.space_size() < size && m_sbuf.resize(m_sbuf.capacity()+ size )!=0)
    {
        return -1 ;
    }


    memcpy(m_sbuf.space(),data + send_len , size  ) ;
    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(m_id.fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    if(delay_flag == 0) on_write(m_id.fd) ;
    return 0 ;
}

int tcp_connection::send( packet *p,int delay_flag)
{
    int size = p->encode_size() ;
    if(m_sbuf.space_size() < size && m_sbuf.resize(m_sbuf.capacity()+ size )!=0)
    {
        return -1 ;
    }

    size = p->encode(m_sbuf.space(),m_sbuf.space_size()) ;
    if ( size < 0 ) return -1 ;

    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(m_id.fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    if(delay_flag == 0) on_write(m_id.fd) ;
    return 0 ;
}

void tcp_connection::handle_error(int error_type)
{
    m_processor->on_error(this,error_type) ;

}

int tcp_connection::get_sock_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getsockname(m_id.fd,(struct sockaddr*)addr,&addr_size) ;

}

int tcp_connection::get_remote_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getpeername(m_id.fd,(struct sockaddr*)addr,&addr_size) ;
}




}

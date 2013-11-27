/*
 * tcp_data_handler.cpp
 *
 *  Created on: 2011-11-2
 *      Author: lxyfirst@yahoo.com.cn
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
//#include <stdio.h>

#include "time_util.h"
#include "tcp_data_handler.h"


namespace kingnet
{

tcp_data_handler::tcp_data_handler():m_reactor(NULL)
{
    // TODO Auto-generated constructor stub

    m_id.fd = -1 ;
    m_id.timestamp = 0 ;
    m_max_write_size = MAX_WRITE_SIZE ;

    m_readall_flag = false;
}

tcp_data_handler::~tcp_data_handler()
{
    fini(true) ;
}

int tcp_data_handler::init(epoll_reactor* reactor,const char* host,int port,int buf_size)
{
    if(m_id.fd >= 0 || host == NULL ) return -1 ;
    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;

    int fd = create_tcp_client(&service_addr,0) ;
    if(fd < 0 ) return -2 ;


    int ret = init(reactor,fd,buf_size) ;
    if( ret != 0 )
    {
        close(fd) ;
    }

    return ret ;
}

int tcp_data_handler::init(epoll_reactor* reactor,int fd,int buf_size)
{
    if(m_id.fd >= 0 || fd < 0 || reactor == NULL ) return -1 ;
    set_nonblock(fd) ;
    //set_socket_nodelay(fd) ;
    if( m_rbuf.resize(buf_size) != 0 ) return -2;
    if( m_sbuf.resize(buf_size) != 0 ) return -2;
    
    if( reactor->add_handler(fd,this,epoll_reactor::EVENT_READ)!=0 )
    {
        return -3 ;
    }

    m_reactor = reactor ;
    m_id.fd = fd ;
    m_id.timestamp = time(0) ;

    m_readall_flag = true ;
    //on_connected() ;
    return 0 ;
}



void tcp_data_handler::fini(bool release)
{
    if(m_id.fd >= 0 )
    {
        //on_disconnect() ;
        m_reactor->del_handler(m_id.fd) ;
        close(m_id.fd) ;
        m_id.fd = -1 ;
        m_id.timestamp = 0 ;
        m_reactor = NULL ;
        m_rbuf.clear() ;
        m_sbuf.clear() ;
    }

    if(release)
    {
        m_rbuf.fini() ;
        m_sbuf.fini() ;
    }
   

}

void tcp_data_handler::on_read(int fd)
{

    do
    {
        int recv_len = recv(fd,m_rbuf.space(),m_rbuf.space_size(),0) ;
        if(recv_len >0)
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
        else if(recv_len == 0)
        {
            //peer close
            handle_error(ERROR_TYPE_PEER_CLOSE) ;
            return ;
        }
        else 
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM)  ;
                return ;
            }

            break ;
        }

    }while(m_readall_flag) ;


}

void tcp_data_handler::on_write(int fd)
{
    if(m_sbuf.data_size() > 0 )
    {
        int to_send = m_sbuf.data_size() > m_max_write_size ? m_max_write_size : m_sbuf.data_size();
        int send_size = ::send(fd,m_sbuf.data(),to_send,0) ;
        if(send_size >0)
        {
            m_sbuf.pop_data(send_size) ;
            m_sbuf.adjust() ;

        }
        else if ( send_size < 0)
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

void tcp_data_handler::on_error(int fd)
{

    handle_error(ERROR_TYPE_SYSTEM) ;

}

int tcp_data_handler::send(const char* data,int size,int delay_flag)
{
    if(m_id.fd < 0 ) return -1 ;
    if(m_sbuf.space_size() < size &&(m_sbuf.resize(m_sbuf.capacity() + size )!=0) )
    {
        return -1 ;
    }

    int send_size = 0 ;
    //try send data directly
    if(m_sbuf.data_size()==0 && delay_flag ==0)
    {
        send_size = ::send(m_id.fd,data,size,0) ;
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

    memcpy(m_sbuf.space(),data + send_size , size) ;
    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(m_id.fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    //if(delay_flag == 0) on_write(m_id.fd) ;

    return 0 ;
}

int tcp_data_handler::send( packet *p,int delay_flag)
{
    if(m_id.fd < 0 ) return -1 ;
    int size = p->encode_size() ;
    if(size < 1 ) return -1 ;
    if(m_sbuf.space_size() < size && m_sbuf.resize(m_sbuf.capacity() + size )!=0)
    {
        return -1 ;
    }

    size = p->encode(m_sbuf.space(),m_sbuf.space_size()) ;
    if ( size < 1 ) return -1 ;
    m_sbuf.push_data(size) ;
    m_reactor->mod_handler(m_id.fd,this,epoll_reactor::EVENT_READ | epoll_reactor::EVENT_WRITE) ;
    if(delay_flag == 0) on_write(m_id.fd) ;

    return 0 ;
}

void tcp_data_handler::handle_error(int error_type)
{
    on_event(error_type) ;

}

int tcp_data_handler::get_sock_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getsockname(m_id.fd,(struct sockaddr*)addr,&addr_size) ;

}

int tcp_data_handler::get_remote_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getpeername(m_id.fd,(struct sockaddr*)addr,&addr_size) ;
}




}

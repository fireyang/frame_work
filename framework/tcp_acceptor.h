/*
 * tcp_acceptor.h
 *
 *  Created on: 2011-10-31
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef TCP_ACCEPTOR_H_
#define TCP_ACCEPTOR_H_

#include <errno.h>
#include <unistd.h>
#include "io_handler.h"
#include "network_util.h"
#include "epoll_reactor.h"

namespace kingnet
{


class tcp_acceptor : public io_handler
{
public:
    enum
    {
        MAX_ACCEPT_ONCE = 32 ,
    };
public:
    tcp_acceptor();
    virtual ~tcp_acceptor();

    /*
     * @brief create listen socket and add to reactor
     * @return 0 if success
     */
    int init(epoll_reactor* reactor,const char* host,int port) ;


    void fini() ;


    int fd() const { return m_fd ; } ;
protected:
    /*
     * @brief  call when accept new fd , implemented by inherent class
     * @return 0 if success
     */
    virtual int on_connection(int fd,sa_in_t* addr) = 0 ;


protected:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;



protected:
    epoll_reactor* m_reactor ;
    int m_fd ;
};

template<typename T,int (T::*callback)(int fd,sa_in_t* addr)=&T::on_connection>
class tcp_service_handler : public io_handler
{
public:
    enum
    {
        MAX_ACCEPT_ONCE = 256 ,
    };


public:
    explicit tcp_service_handler():m_owner(NULL){ } ;
    virtual ~tcp_service_handler() { } ;

    /*
     * @brief owner must implement below interface
     * int on_connection(int cfd,sa_in_t* caddr)
     */
    void set_owner(T* owner)
    {
        m_owner = owner ;

    } ;

protected:
    virtual void on_read(int fd)
    {
        socklen_t addr_len;
        sa_in_t caddr = {0};
        addr_len = sizeof(caddr);

        int cfd = 0 ;
        for(int i = 0 ; i< MAX_ACCEPT_ONCE ; ++i )
        {
            cfd = accept(fd,(sa_t*)&caddr,&addr_len) ;
            if(cfd >= 0 )
            {
                if ( (m_owner->*callback)(cfd,&caddr) != 0 ) close(cfd) ;
            }
            else
            {
                if(errno == EAGAIN || errno ==EMFILE || errno ==ENFILE ) break ;
                else if ( errno == EINTR || errno ==ECONNABORTED ) continue ;
                else on_error(fd);

            }
        }

    }
    virtual void on_write(int fd) { } ;

    virtual void on_error(int fd) { close(fd) ; } ;


protected:
    T* m_owner ;

};


}

#endif /* TCP_ACCEPTOR_H_ */

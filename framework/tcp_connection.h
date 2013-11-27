/*
 * tcp_connection.h
 *
 *  Created on: 2011-11-2
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include "io_handler.h"
#include "network_util.h"
#include "epoll_reactor.h"
#include "buffer.h"


namespace kingnet
{


class packet_processor ;
class packet ;



class tcp_connection : public io_handler
{
public:
    struct connection_id
    {
        int fd ;
        int timestamp ;

        bool operator!=(const connection_id& o) const
        {
            return ( fd!=o.fd || timestamp != o.timestamp) ;
        }

        bool operator==(const connection_id& o) const
        {
            return ( fd==o.fd && timestamp == o.timestamp) ;
        }

    }  ;
    //enum
    //{
    //    ERROR_TYPE_NONE = 0 ,
    //    ERROR_TYPE_SYSTEM = 1 ,
    //    ERROR_TYPE_MEMORY = 2 ,
    //    ERROR_TYPE_REQUEST = 3 ,
    //    ERROR_TYPE_TIMEOUT = 4 ,
    //    ERROR_TYPE_PEER_CLOSE = 5 ,
    //};
public:
    tcp_connection();
    virtual ~tcp_connection();


public:
    /*
     * @brief initialize tcp_connection , for active connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] remote address to connect
     * @param [in] packet processor which should decode and consume packet
     * @return 0 on sucess , nagtive value on failure
     */
    int init(epoll_reactor* reactor,const char* host,int port,packet_processor *ph) ;
    /*
     * @brief initialize tcp_connection ,for passive connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] fd for monitor
     * @param [in] packet processor which should decode and consume packet
     * @return 0 on sucess , nagtive value on failure
     */
    int init(epoll_reactor* reactor,int fd ,packet_processor *ph) ;

    void fini() ;



    /*
     * @brief get connection_id
     */
    const connection_id& get_id() const { return m_id ; } ;
    bool connected() const { return m_id.fd != -1 ; } ;

    int get_errno() const { return get_socket_error(m_id.fd) ; } ;

    /*
     * @brief send data
     * @return 0 on success , -1 on failure
     */
    int send(const char* data,int size,int delay_flag) ;

    /*
     * @brief send packet
     * @return 0 on success , -1 on failure
     */
    int send(packet * p,int delay_flag) ;

    int get_sock_addr(sa_in_t* addr) const ;

    int get_remote_addr(sa_in_t* addr) const ;


protected:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;

    void handle_error(int error_type) ;

private:
    buffer m_rbuf ;
    buffer m_sbuf ;

    connection_id m_id ;
    epoll_reactor* m_reactor ;
    packet_processor *m_processor ;



};




}

#endif /* TCP_CONNECTION_H_ */

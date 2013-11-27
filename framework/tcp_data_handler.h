/*
 * tcp_data_handler.h
 *
 *  Created on: 2011-11-2
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef TCP_DATA_HANDLER_H_
#define TCP_DATA_HANDLER_H_

#include "io_handler.h"
#include "network_util.h"
#include "epoll_reactor.h"
#include "buffer.h"
#include "packet.h"

namespace kingnet
{


class tcp_data_handler : public io_handler
{
public:
    struct connection_id
    {
        int fd ;
        int timestamp ;

        bool operator!=(const connection_id& o)
        {
            return ( fd!=o.fd || timestamp != o.timestamp) ;
        }

        bool operator==(const connection_id& o)
        {
            return ( fd==o.fd && timestamp == o.timestamp) ;
        }

    }  ;
	/*enum
	{
	ERROR_TYPE_NONE = 0 ,
	ERROR_TYPE_SYSTEM = 1 ,
	ERROR_TYPE_MEMORY = 2 ,
	ERROR_TYPE_REQUEST = 3 ,
	ERROR_TYPE_TIMEOUT = 4 ,
	ERROR_TYPE_PEER_CLOSE = 5 ,
	};*/

    enum
    {
        INIT_RECV_BUF_SIZE = 81920 ,
        INIT_SEND_BUF_SIZE = 81920 ,
        INIT_BUF_SIZE = 81920 ,
    };

    enum
    {
        MIN_WRITE_SIZE = 0x1 << 10 ,
        MAX_WRITE_SIZE = 0x1 << 24 ,
    };

public:
    tcp_data_handler();
    virtual ~tcp_data_handler();


public:
    /*
     * @brief initialize connection , for active connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] remote address to connect
     * @return 0 on sucess , nagtive value on failure
     */
    int init(epoll_reactor* reactor,const char* host,int port,int buf_size = INIT_BUF_SIZE) ;
    /*
     * @brief initialize connection ,for passive connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] fd for monitor
     * @return 0 on sucess , nagtive value on failure
     */
    int init(epoll_reactor* reactor,int fd,int buf_size=INIT_BUF_SIZE ) ;

    /*
     * @brief clear connection and buffer
     * @param [in] release buffer memroy flag
     */
    void fini(bool release=false) ;

    /*
     * @brief get connection_id
     */
    const connection_id& get_id() const { return m_id ; } ;
    bool connected() { return m_id.fd != -1 ; } ;

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
    int send(packet * p,int delay_flag = 0) ;

    int get_sock_addr(sa_in_t* addr) const ;

    int get_remote_addr(sa_in_t* addr) const ;
    
    void set_readall_flag(bool flag) { m_readall_flag = flag ; } ;
    void set_max_write_size(int max_size) 
    { 
        if(max_size < MIN_WRITE_SIZE) max_size = MIN_WRITE_SIZE ;
        m_max_write_size = max_size ; 
    } ;
    
protected:
    //virtual void on_connected() { } ;
    //virtual void on_disconnect() { } ;

    /*
     * @brief handle exceptional event , implemented by concrete class
     * @param [in] exception type
     */
    virtual void on_event(int type) =  0 ;

    /*
     * @brief get packet info , implemented by concrete class
     * @param [in] buffer pointer
     * @param [in] buffer size
     * @param [out] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int get_packet_info(const char* data,int size,packet_info* pi) = 0 ;

    /*
     * @brief process packet callback , implemented by concrete class
     * @param [in] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int process_packet(const packet_info* pi) = 0 ;


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
    int m_max_write_size ;
    bool m_readall_flag ;

};




}

#endif /* TCP_DATA_HANDLER_H_ */

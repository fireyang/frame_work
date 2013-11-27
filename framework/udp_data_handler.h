/*******************************************************************************
 *
 *      @file: udp_data_handler.h
 *      @brief: handle read/write udp data from/to network.
 *
 *      @author: gxl.
 *      @Date: 2012/10/08
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#ifndef UDP_DATA_HANDLER_H_
#define UDP_DATA_HANDLER_H_

#include "io_handler.h"
#include "network_util.h"
#include "epoll_reactor.h"
#include "buffer.h"
#include "packet.h"

namespace kingnet
{


class udp_data_handler : public io_handler
{
public:
    enum
    {
        INIT_RECV_BUF_SIZE = 4096 ,
        INIT_SEND_BUF_SIZE = 8192 ,
    };

public:
    udp_data_handler();
    virtual ~udp_data_handler();


public:
    /*
     * @brief initialize connection , for active connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] remote address to connect
     * @return 0 on sucess , negative value on failure
     */
    int init(epoll_reactor* reactor,const char* host,int port) ;
    /*
     * @brief initialize connection from a initialized fd
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] fd for monitor
     * @return 0 on sucess , negative value on failure
     */
    int init(epoll_reactor* reactor,int fd ) ;

    void fini() ;


    int get_errno() const { return get_socket_error(sock_fd) ; } ;

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

    int get_port() const { return m_port; }

protected:

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
	sa_in_t service_addr ;
	int     sock_fd;
    int     m_port;

    buffer m_rbuf ;
    buffer m_sbuf ;
    //connection_id m_id ;
    epoll_reactor* m_reactor ;

};




}

#endif /* udp_DATA_HANDLER_H_ */

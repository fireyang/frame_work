/*******************************************************************************
 *
 *      @file: udp_server.h
 *      @brief: bind to a port as a service, 
                also, handle read/write udp data from/to network.
 *
 *      @author: gxl.
 *      @Date: 2012/10/08
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include <errno.h>
#include <unistd.h>
#include "io_handler.h"
#include "buffer.h"
#include "network_util.h"
#include "epoll_reactor.h"
#include "packet.h"

namespace kingnet
{


class udp_server : public io_handler
{
public:
	enum
	{
		INIT_RECV_BUF_SIZE = 4096 ,
		INIT_SEND_BUF_SIZE = 8192 ,
	};

public:
    udp_server();
    virtual ~udp_server();

    /*
     * @brief create listen socket and add to reactor
     * @return 0 if success
     */
    int init(epoll_reactor* reactor,const char* host,int port) ;


    void fini() ;


    int fd() const { return m_fd ; } ;

	int get_errno() const { return get_socket_error(m_fd) ; } ;

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


protected:

    /*
     * @brief handle exceptional event , implemented by concrete class
     * @param [in] exception type
     */
    virtual void on_event(int type);

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

protected:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;

	void handle_error(int error_type) ;

protected:
    epoll_reactor* m_reactor ;
    int m_fd ;
	buffer m_rbuf ;
	buffer m_sbuf ;
};

}

#endif /* udp_server_H_ */


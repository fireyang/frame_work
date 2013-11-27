/*******************************************************************************
 *
 *      @file: udp_server.cpp
 *      @brief: bind to a port as a service, 
                also, handle read/write udp data from/to network.
 *
 *      @author: gxl.
 *      @Date: 2012/10/08
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "udp_server.h"

namespace kingnet
{

	udp_server::udp_server():m_reactor(NULL),m_fd(-1)
	{
	}

	udp_server::~udp_server()
	{
		fini() ;
	}

	int udp_server::init(epoll_reactor* reactor,const char* host,int port)
	{
		if(m_fd >= 0 || reactor == NULL || host == NULL ) return -1 ;
		sa_in_t service_addr ;
		init_sa_in(&service_addr,host,port) ;
		int sfd = create_udp_service(&service_addr) ;
		if(sfd <0 ) return -2 ;

		if( reactor->add_handler(sfd, this, epoll_reactor::EVENT_READ)!=0 )
		{
			close(sfd) ;
			return -3 ;
		}

		m_rbuf.init(INIT_RECV_BUF_SIZE) ;
		m_sbuf.init(INIT_SEND_BUF_SIZE) ;
		if(m_rbuf.space() == NULL || m_sbuf.space() == NULL )
		{
			return -2 ;
		}

		m_reactor = reactor ;
		m_fd = sfd ;

		return 0 ;

	}

	void udp_server::fini()
	{
		if(m_fd >= 0 )
		{
			m_reactor->del_handler(m_fd) ;
			close(m_fd) ;
			m_fd = -1 ;

		}
	}

	void udp_server::handle_error(int error_type)
	{
		on_event(error_type) ;
	}

	void udp_server::on_read(int fd)
	{
		struct sockaddr_in  clientaddr;
		socklen_t  addrlen = sizeof(clientaddr);

		size_t recv_len = recvfrom(fd, m_rbuf.space(), m_rbuf.space_size(), 0,
									(sockaddr*)&clientaddr, &addrlen) ;

		//fprintf(stderr, "recv %d bytes data.\n", recv_len);

		if(recv_len > 0)
		{
			m_rbuf.push_data(recv_len) ;
			while(m_rbuf.data_size() > 0)
			{
				packet_info pi = {0} ;
				int ret = 0 ;
				if((ret = get_packet_info(m_rbuf.data(), m_rbuf.data_size(), &pi))!=0)
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
					if((ret= process_packet(&pi)) !=0 )
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

	void udp_server::on_write(int fd)
	{
		// Not implemented.
		// Use Direct write.
	}

	void udp_server::on_error(int fd)
	{
		fini() ;
	}

	void udp_server::on_event(int type)
	{
		//LOG_INFO("client on_event:%d fd:%d errno:%d", type, m_fd, get_errno()) ;

		fini() ;
	}
	
}

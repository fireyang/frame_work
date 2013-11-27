/*
 * network_util.h
 *
 *  Created on: 2011-10-14
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>

namespace kingnet
{

//in_addr_t
typedef struct ::sockaddr_in sa_in_t ;
typedef struct ::sockaddr   sa_t ;
typedef struct ::sockaddr_un sa_un_t ;


/**
 *@brief init address
 *@param [out] address
 *@param [in] ip  string
 *@param [in] port
 */
void init_sa_in(sa_in_t* addr,const char* ip,int port) ;
int compare_sa_in(sa_in_t* addr1,sa_in_t* addr2);
void init_sa_un(sa_un_t* addr,const char* sockfile) ;

int set_nonblock(int fd) ;

void set_socket_buffer(int fd,int val);

int set_addr_reuse(int fd) ;

int set_socket_nodelay(int fd) ;

int set_defer_accept(int fd,int seconds) ;

int get_socket_error(int fd) ;


/** socket,bind,listen  */
int create_tcp_service(sa_in_t* addr) ;
int create_udp_service(sa_in_t* addr) ;

/** socket , connect */
int create_tcp_client(sa_in_t* remote_addr,int timeout) ;
int create_udp_client(sa_in_t* remote_addr,int timeout) ;
int create_unix_client(sa_un_t* remote_addr,int timeout) ;


ssize_t send_wrapper(int __fd, __const void *__buf, size_t __n, int __flags) ;

}

#endif /* NETWORK_UTIL_H_ */

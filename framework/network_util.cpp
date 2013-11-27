/*
 * network_util.h
 *
 *  Created on: 2011-10-14
 *      Author: lxyfirst@yahoo.com.cn
 */

#define _XOPEN_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "network_util.h"

namespace kingnet
{

ssize_t send_wrapper(int __fd, __const void *__buf, size_t __n, int __flags)
{
	ssize_t offset = 0;
	size_t  total = __n;
	ssize_t len_sent = 0;
	while ((len_sent = send(__fd, (const void *)((const char*)__buf + offset), total, __flags)) > 0) {
		if (len_sent < (ssize_t)total) {
			offset += len_sent;
			total -= len_sent;

		}else{
			return __n;
		}
	}

	return len_sent;
}


void init_sa_in(sa_in_t* addr,const char* ip,int port)
{
    if ( addr == NULL || ip == NULL || port < 1 ) return  ;
    memset(addr,0,sizeof(sa_in_t)) ;
    addr->sin_family = AF_INET ;
    addr->sin_port = htons(port) ;
    addr->sin_addr.s_addr = inet_addr(ip) ;
}

int compare_sa_in(sa_in_t* addr1,sa_in_t* addr2)
{
    if (addr1->sin_port == addr2->sin_port && 
        addr1->sin_addr.s_addr == addr2->sin_addr.s_addr ) 
    {
        return 0 ;
    }
        

    return -1 ;
}


void init_sa_un(sa_un_t* addr,const char* file)
{
    if ( addr == NULL || file == NULL ) return  ;
    memset(addr,0,sizeof(sa_un_t)) ;
    addr->sun_family = AF_LOCAL ;
    strncpy(addr->sun_path,file,sizeof(addr->sun_path)-1) ;
}

int set_nonblock(int fd)
{
    int flag = fcntl(fd,F_GETFL) ;
    if ( flag == -1)
    {
        return -1 ;
    }

    return fcntl(fd,F_SETFL,flag | O_NONBLOCK) ;
}

void set_socket_buffer(int fd,int val)
{
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val)) ;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val)) ;
}

int set_addr_reuse(int fd)
{
    int reuse = 1 ;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) ;

}


int set_socket_nodelay(int fd)
{
    int one = 1 ;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &one, sizeof(one)) ;
}

int set_defer_accept(int fd,int seconds)
{
    return setsockopt(fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, (char *) &seconds, sizeof(seconds)) ;    
}



int get_socket_error(int fd)
{
    int error_code = 0 ;
    socklen_t len = (socklen_t)sizeof(error_code);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &error_code, &len);

    return error_code ;
}




int create_tcp_service(sa_in_t* addr)
{
    if ( addr == NULL  ) return -1 ;
    int sockfd = socket(AF_INET,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    set_addr_reuse(sockfd) ;
    if( bind(sockfd,(sa_t*)addr,sizeof(sa_in_t)) !=0 )
    {
        close(sockfd) ;
        return -2 ;
    }
    if ( listen(sockfd,1024) != 0 )
    {
        close(sockfd) ;
        return -2 ;
    }

    return sockfd ;

}

int create_udp_service(sa_in_t* addr)
{
    if ( addr == NULL ) return -1 ;
    int sockfd = socket(AF_INET,SOCK_DGRAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    set_addr_reuse(sockfd) ;
    if( bind(sockfd,(sa_t*)addr,sizeof(sa_in_t)) !=0 )
    {
        close(sockfd) ;
        return -2 ;
    }
    

    return sockfd ;

}

static int check_socket_connect(int sockfd ,int second)
{

    struct timeval timeout= {second,0} ;
    fd_set rs,ws ;
    FD_ZERO(&rs) ;
    FD_ZERO(&ws) ;
    FD_SET(sockfd,&rs) ;
    FD_SET(sockfd,&ws) ;
    if ( select(sockfd+1,&rs,&ws,NULL,&timeout)  < 1 )
    {
        errno = ETIMEDOUT ;
        return -1 ;
    }

    errno = get_socket_error(sockfd) ;
    if ( errno!=0 )
    {
        return -1 ;
    }

    return 0 ;

}

int create_tcp_client(sa_in_t* addr,int second)
{
    if ( addr == NULL ) return -1 ;

    int sockfd = socket(AF_INET,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;

    if ( connect(sockfd,(sa_t*)addr,sizeof(sa_in_t)) != 0 && errno != EINPROGRESS )
    {
        close(sockfd) ;
        return -errno ;

    }

    if(second == 0) return sockfd ;

    if(check_socket_connect(sockfd,second)!=0)
    {
        close(sockfd) ;
        return -errno ;
    }

    return sockfd ;

}

int create_udp_client(sa_in_t* addr,int second)
{
    if ( addr == NULL ) return -1 ;


    int sockfd = socket(AF_INET,SOCK_DGRAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;

    if ( connect(sockfd,(sa_t*)addr,sizeof(sa_in_t)) != 0 && errno != EINPROGRESS )
    {
        close(sockfd) ;
        return -errno ;
    }

    if(second == 0) return sockfd ;

    if(check_socket_connect(sockfd, second)!=0)
    {
        close(sockfd) ;
        return -errno ;
    }

    return sockfd ;
}

int create_unix_client(sa_un_t* addr,int second)
{
    if ( addr == NULL ) return -1 ;


    int sockfd = socket(AF_LOCAL,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    if ( connect(sockfd,(sa_t*)addr,sizeof(sa_un_t)) != 0 && errno != EINPROGRESS )
    {
        close(sockfd) ;
        return -errno ;
    }

    if(second == 0) return sockfd ;

    if(check_socket_connect(sockfd,second)!=0)
    {
        close(sockfd) ;
        return -errno ;
    }

    return sockfd ;
}

}


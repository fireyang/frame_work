/*
 * io_handler.h
 *
 *  Created on: 2011-10-14
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_



namespace kingnet
{

	enum
	{
		ERROR_TYPE_NONE = 0 ,
		ERROR_TYPE_SYSTEM = 1 ,
		ERROR_TYPE_MEMORY = 2 ,
		ERROR_TYPE_REQUEST = 3 ,
		ERROR_TYPE_TIMEOUT = 4 ,
		ERROR_TYPE_PEER_CLOSE = 5 ,
		ERROR_TYPE_ACTIVE_CLOSE = 6 ,
	};


/*
 * @brief  interface for event driven handler
 */
class io_handler
{
    friend class epoll_reactor ;

public:
    io_handler(){ } ;
    virtual ~io_handler() { } ;


protected :

    /*
     * @brief error events callback , implemented by concrete class
     */
    virtual void on_error(int fd) = 0 ;


    /*
     * @brief read events callback , implemented by concrete class
     */
    virtual void on_read(int fd) = 0 ;


    /*
     * @brief write events callback , implemented by concrete class
     */
    virtual void on_write(int fd) = 0 ;

private:
    io_handler(const io_handler& ) ;
    io_handler& operator=(const io_handler&) ;


};

}

#endif /* IO_HANDLER_H_ */


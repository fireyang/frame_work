/*
 * notify_handler.h
 *
 *  Created on: 2011-10-22
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef NOTIFY_HANDLER_H
#define NOTIFY_HANDLER_H

#include <stdlib.h>
#include <stdint.h>

#include "io_handler.h"

namespace kingnet
{

/*
 * @brief notify handler for pipe
 */
class notify_handler : public io_handler
{

public:
    notify_handler() : m_fd(-1) { } ;
    virtual ~notify_handler() { } ;

    int fd() const { return m_fd ;} ;
    void set_fd(int fd) { m_fd = fd ; } ;

    /*
     * @brief send notify signal to pipe
     */
    int send_notify() ;


protected:
    /*
     * @brief callback implemented by concrete class
     * called when get notify signal
     */
    virtual void on_notify() = 0 ;

protected:

    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;


private:
    int32_t m_fd ;

    
} ;




}

#endif




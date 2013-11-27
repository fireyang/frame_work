/*
 * packet_processor.h
 *
 *  Created on: 2011-12-1
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef PACKET_PROCESSOR_H_
#define PACKET_PROCESSOR_H_

namespace kingnet
{

class tcp_connection ;
struct packet_info ;

class packet_processor
{
public:
    virtual ~packet_processor() { } ;


    /*
     * @brief get packet info , implemented by concrete class
     * @param [in] buffer pointer
     * @param [in] buffer size
     * @param [out] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int get_info(const char* data,int size,packet_info* pi) = 0 ;


    /*
     * @brief process packet callback , implemented by concrete class
     * @param [in] connection pointer
     * @param [in] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int process(tcp_connection* conn,const packet_info* pi) = 0 ;

    /*
     * @brief connect event callllback , implemented by concrete class
     * @param [in] connection pointer
     * @param [in] reason
     */
    virtual void on_connected(tcp_connection* conn) = 0 ;
    

    /*
     * @brief disconnect event callllback , implemented by concrete class
     * @param [in] connection pointer
     * @param [in] reason
     */
    virtual void on_disconnect(tcp_connection* conn ) = 0 ;

    virtual void on_error(tcp_connection* conn,int error_type) = 0 ;

};

}

#endif /* PACKET_PROCESSOR_H_ */

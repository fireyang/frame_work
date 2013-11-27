/*
 * client_connection.h
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include <string>
#include "tcp_data_handler.h"
#include "timer_manager.h"
#include "threshold.h"

using kingnet::timer_manager ;
using kingnet::template_timer ;


using kingnet::packet_info ;
using kingnet::tcp_data_handler ;

class ClientConnection : public tcp_data_handler
{
public:
    enum
    {
		STATUS_DEFAULT = 0,
        STATUS_PREPARE = 1 ,
        STATUS_ACC_LOGIN = 2 ,
        STATUS_PLAYER_LOGIN = 3,
        STATUS_IN_GAME = 4 ,
    };
    enum { MAX_ACTION_SIZE = 512 , } ;
public:
    ClientConnection() ;
    virtual ~ClientConnection();

    void on_connect() ;
    void on_player_login() ;
    void on_acc_login(int role_id) ;
    void on_idle_timeout(timer_manager* manager);

    int get_status() const { return m_status ; } ;
    int get_uid() const { return m_uid ; } ;
    int get_role_id() const { return m_role_id ; } ;

	void  set_login_ip(const std::string& ip) { m_last_ip = ip; }

	void close()
	{
		on_event(0);
	}
	void set_status(int status)
	{
	    m_status = status;
	}
	int server_id()
	{
		return m_server_id;
	}

	void reset();
protected:
    virtual void on_event(int type) ;
    virtual int get_packet_info(const char* data,int size,packet_info* pi);
    virtual int process_packet(const packet_info* pi) ;


	int process_game_packet(const packet_info* pi) ;
    int forward_packet(const packet_info* pi,int32_t key) ;
    int process_player_login(const packet_info* pi);
	//
	int32_t get_next_seq() { return ++m_seq_counter; }
	// void send_login_log(bool online = true);

    bool check_seq(int seq) ; 
    bool check_heart(int value) ; 
    bool check_threshold(int msg_id) ; 
private:
    template_timer<ClientConnection,&ClientConnection::on_idle_timeout> m_idle_timer ;
	int32_t m_seq_counter;
    int32_t m_uid ;
    int32_t m_role_id ;
	int32_t m_server_id;
    int32_t m_status ;
    int32_t m_policy_status ;
    int64_t m_last_time ;
	std::string  m_last_ip ; // 最后一次登陆ip
    int32_t m_client_seq ;
    int32_t m_last_heart ;
    int32_t m_cur_heart ;
    int32_t m_heart_time ;
	Threshold m_threshold[MAX_ACTION_SIZE] ;
	//bool    m_is_first_log ; 
};

#endif /* CLIENT_CONNECTION_H_ */

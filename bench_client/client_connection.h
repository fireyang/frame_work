/*
 * client_connection.h
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "tcp_data_handler.h"
#include "game_common.pb.h"
#include "timer_manager.h"


using kingnet::timer_manager ;
using kingnet::template_timer ;
using kingnet::packet_info ;
using kingnet::tcp_data_handler ;

typedef struct
{
    int map_id ;
    int map_x ;
    int map_y ;
    int npc_id ;

} npc_info ;

class client_connection : public tcp_data_handler
{
public:
    enum
    {
        STATUS_ACC_LOGIN = 1 ,
        STATUS_CREATE_PLAYER = 2 ,
        STATUS_PLAYER_LOGIN = 3,
        STATUS_ENTER_MAP = 4,
        STATUS_IN_GAME = 5 ,
        STATUS_TELEPORT = 6 ,
    };
public:
    client_connection() ;
    virtual ~client_connection();

    void send_acc_login() ;
    void run(int uid,int delay_ms)  ;
protected:
    virtual void on_event(int type) ;
    virtual int get_packet_info(const char* data,int size,packet_info* pi);
    virtual int process_packet(const packet_info* pi) ;

    int process_game_packet_response(const packet_info* pi) ;
    int process_player_login_response(const packet_info* pi) ;

    void send_player_login() ;

    void on_timeout(timer_manager* manager) ;

private:
    template_timer<client_connection> m_timer ;
    int32_t m_uid ;
    int32_t m_delt_x ;
    int32_t m_delt_y ;
    int32_t m_count ;
    int32_t m_status ;
    int32_t m_error_count ;
    bool m_first_enter_map ;
    npc_info m_npc_info ;
    bool has_response;

	enum {
		quest_id_range_min = 1001,
		quest_id_range_max = 5001,
	};
	int m_quest_id;
	int m_quest_status;
	int m_heart_time ;
	char m_cmd_str[1024];

	bool gm_executed;
	char m_gm_str[1024];
};

#endif /* CLIENT_CONNECTION_H_ */

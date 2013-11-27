/*
 * client_connection.cpp
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */
#include <stdlib.h>
#include "client_connection.h"
#include "message.h"
#include "client_application.h"
#include "../public/md5.h"
#include "../public/str_util.h"
#include <iostream>
#include <string>
using namespace std;


client_connection::client_connection()
	:m_uid(0)
	,m_status(STATUS_ACC_LOGIN)
	, m_quest_id(quest_id_range_min)
	, m_quest_status(1)
{
    m_timer.set_owner(this) ;
    // TODO Auto-generated constructor stub
	snprintf(m_cmd_str, sizeof(m_cmd_str), "%s", get_app().quest_str());
	snprintf(m_gm_str, sizeof(m_gm_str), "%s", get_app().gm_str());
}

client_connection::~client_connection()
{
    // TODO Auto-generated destructor stub
}


void client_connection::run(int uid,int delay_ms)
{
    m_uid = uid ;
    m_delt_x = 0 ;
    m_delt_y =0 ;
    m_count = 0 ;
    m_status =STATUS_ACC_LOGIN ;
    m_error_count = 0 ;
    gm_executed = false;
    has_response = true;

    m_timer.set_expired(get_app().get_run_ms() + delay_ms) ;
    get_app().timer_engine.add_timer(&m_timer) ;

}


void client_connection::on_event(int type)
{
    LOG_INFO("client on_event:%d fd:%d errno:%d",type,get_id().fd,get_errno()) ;

    m_delt_x = 0 ;
    m_delt_y =0 ;
    m_status =STATUS_ACC_LOGIN ;

    fini() ;
}
int client_connection::get_packet_info(const char* data,int size,packet_info* pi)
{

    if(size < (int)sizeof(cs_head) )
    {
        pi->size = (int)sizeof(cs_head) ;
    }
    else
    {
        cs_head* head = (cs_head*)data ;
        pi->type = ntoh_int16(head->msgid) ;
        pi->size = ntoh_int16(head->length) ;
        pi->data = data ;
    }

    LOG_DEBUG("client recv type:%d size:%d real_size:%d",pi->type,pi->size,size) ;

    return 0 ;
}
int client_connection::process_packet(const packet_info* pi)
{

    switch(pi->type)
    {
    case cs_player_login_response::packet_type:
        if(m_status == STATUS_PLAYER_LOGIN) return process_player_login_response(pi) ;
        break ;
    
    default:
        return process_game_packet_response(pi) ;
    }
        
    return -1 ;
}



int client_connection::process_game_packet_response(const packet_info* pi)
{
    return 0 ;
}

int client_connection::process_player_login_response(const packet_info* pi)
{
    info_log_format(get_app().logger,"get login response");
    return 1;
}


void client_connection::send_player_login()
{
    //send(&request,0) ;
    cs_player_login_request request;
    request.body.set_uid(time(NULL)%1000);
    request.body.set_pwd("nice_body");
    if(send(&request,0))
    {
        info_log_format(get_app().logger,"send login ");
    }
    info_log_format(get_app().logger,"send login success %d",time(NULL)%1000);
    m_status = STATUS_PLAYER_LOGIN ;
} 



void client_connection::on_timeout(timer_manager* manager)
{
    send_player_login();
    m_timer.set_expired(get_app().get_run_ms() + 5000);
    manager->add_timer(&m_timer);
}


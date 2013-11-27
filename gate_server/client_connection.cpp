/*
 * client_connection.cpp
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#include "client_connection.h"
#include "message.h"
#include "gate_application.h"
#include "gate_application.h"
#include "common_util.h"
#include "../public/str_util.h"
#include "../public/md5.h"

using namespace std;

ClientConnection::ClientConnection()
	: m_seq_counter(0), m_uid(0), m_role_id(0)
	, m_server_id(0)
	, m_status(STATUS_DEFAULT)
	, m_last_time(0)
	, m_last_ip("")
	,m_client_seq(0)
{
    m_policy_status = 0 ;
    m_idle_timer.set_owner(this);

}

ClientConnection::~ClientConnection()
{
    // TODO Auto-generated destructor stub
}

void ClientConnection::reset()
{
	m_seq_counter = 0;
	m_uid = 0 ;
	m_role_id = 0 ;
	m_server_id = 0;
	m_status = STATUS_DEFAULT ;
	m_last_time = 0 ;
	m_last_ip = "" ; // 最后一次登陆ip
	m_client_seq = 0 ;
	m_last_heart = 0 ;
	m_cur_heart = 0 ;
	m_heart_time = 0 ;
	memset(m_threshold, 0, sizeof(m_threshold));

	get_app().timer_engine.del_timer(&m_idle_timer);
}

void ClientConnection::on_event(int type)
{
    LOG_INFO("close client ip:%s fd:%d uid:%d role_id:%d type:%d errno:%d",
        m_last_ip.c_str(),get_id().fd,m_uid,m_role_id,type,get_errno());

    if(m_uid > 0)
    {
		get_app().set_user_connection(m_uid,m_server_id, 0) ;
		if( m_role_id > 0)
		{
			/*ss_player_logout_notify logout ;
			logout.key = m_role_id ;
			logout.body.set_guid(m_role_id) ;
			get_app().zone_conn.send(&logout,0) ;
*/
            get_app().set_role_connection(m_role_id,0) ;
		}		
    }

    m_status = STATUS_DEFAULT ;

	reset() ;

    fini() ;
    get_app().free_connection(this) ;
}
int ClientConnection::get_packet_info(const char* data,int size,packet_info* pi)
{
    static const char POLICY_REQUEST[] ="<policy-file-request/>" ;
    if( m_policy_status == 0 && memcmp(data,POLICY_REQUEST,sizeof(POLICY_REQUEST)-1 )==0 )
    {
        m_policy_status = 1 ;
        pi->data = data ;
        pi->size = size ;
        return 0 ;
    }

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

    if(pi->size < (int)sizeof(cs_head) || pi->size > 10240 ) return -4 ;

    return 0 ;
}


int ClientConnection::process_packet(const packet_info* pi)
{
    if(m_policy_status == 1 )
    {
        static const char POLICY_RESPONSE[] = "<cross-domain-policy>"
            "<allow-access-from domain='*' to-ports='*'/>"
            "</cross-domain-policy>" ;
        send(POLICY_RESPONSE,sizeof(POLICY_RESPONSE),0) ;
        m_policy_status = 2 ;
        return 0 ;
    }
    
    if(get_app().check_threshold && (!check_threshold(pi->type)) )
    {
        LOG_DEBUG("threshold limit uid:%d role_id:%d action:%d",
            m_uid, m_role_id, pi->type >> 2 ) ;
        return 0 ;
    }

    if(get_app().check_seq && 
        (!check_seq(ntoh_int32(((cs_head_plus*)(pi->data))->seq))) ) return -1 ;

    //if(get_app().check_heartbeat && 
     //   (!check_heart(ntoh_int16(((cs_head_plus*)(pi->data))->reserved))) ) return -2 ;

    int t_m_status = m_status;
   switch(pi->type)
    {
    case cs_player_login_request::packet_type:
        return process_player_login(pi) ;
        break ;
    }
    int pack_type =int(pi->type >> 2);
	LOG_WARN( "unexpected packet type: %d with client status: %d -- %d ", pack_type , int(t_m_status), int(STATUS_IN_GAME));
    //return -3 ;
    return 0 ;
}

int ClientConnection::process_player_login(const packet_info* pi)
{
    cs_player_login_request request;
    if(request.decode(pi->data,pi->size)!=pi->size)return -1;
    string pwd = request.body.pwd();
    LOG_INFO("process player login request %d:%s",request.body.uid(),pwd.c_str());

    cs_player_login_response response;
    response.body.set_error_code(0);
    send(&response,0);

    return 0;
}


void ClientConnection::on_idle_timeout(timer_manager* manager)
{
    if(m_status < STATUS_PREPARE )
    {
        LOG_WARN("close idle connection uid:%d role_id:%d",m_uid,m_role_id);
        on_event(0);
    }
    else
    {
        if(time(0) - m_heart_time > 180 )
        {
            LOG_WARN("close idle connection uid:%d role_id:%d",m_uid,m_role_id);
            on_event(0);
        }
        else
        {
            //active heartbeat
            if(get_app().active_heartbeat)
            {
                if(time(0) - m_heart_time >=100) 
                {
                    LOG_DEBUG("active_heartbeat uid:%d role_id:%d",m_uid,m_role_id);
                }
/*                cs_heartbeat_notify notify ;
                this->send(&notify, 0);*/
            }

            m_idle_timer.set_expired(get_app().get_run_ms() + 50000 ) ;
            if(get_app().timer_engine.add_timer(&m_idle_timer)!=0) return ;
        }
    }

}

void ClientConnection::on_connect()
{
    m_client_seq = 0 ;
    m_cur_heart = m_last_heart = 0 ;
    m_heart_time = time(0) ;
    m_status = STATUS_DEFAULT ;
    m_policy_status = 0 ;
    for(int i = 0 ; i < MAX_ACTION_SIZE ; ++i)
    {
        m_threshold[i].init(1,50) ;
    }

    //m_idle_timer.set_expired(get_app().get_run_ms() + 20000 ) ;
    m_idle_timer.set_expired(get_app().get_run_ms() + 5000 ) ;
    if(get_app().timer_engine.add_timer(&m_idle_timer)!=0) return ;

}

void ClientConnection::on_player_login()
{
    m_status = STATUS_IN_GAME ;

}

void ClientConnection::on_acc_login(int role_id)
{
    m_role_id = role_id ;
    m_status = STATUS_PLAYER_LOGIN;
}




int ClientConnection::process_game_packet(const packet_info* pi)
{
    return get_app().forward_cs_packet(pi,m_role_id) ;

}

int ClientConnection::forward_packet(const packet_info* pi,int32_t key)
{
    ss_head head = {0};
    head.key = hton_int32(key) ;
    head.seq = ((cs_head_plus*)(pi->data))->seq ;
    head.msgid = hton_int16(pi->type) ;
    head.length = hton_int16(pi->size - cs_head_size(pi->type) + sizeof(head) ) ;
	head.tick = kingnet::time_ms();
    //get_app().zone_conn.send((const char*)&head,sizeof(head),0) ;
   /// get_app().zone_conn.send(pi->data + cs_head_size(pi->type),pi->size - cs_head_size(pi->type),0) ;

    return 0 ;


}


bool ClientConnection::check_seq(int seq)
{
    if(m_client_seq >= seq)
    {
        LOG_WARN("invalid seq role_id:%d seq:%d",m_role_id,seq) ;
        return false ;
    }

    m_client_seq = seq ;
    return true ;
}


bool ClientConnection::check_heart(int value)
{
    if(value == m_cur_heart || value == m_last_heart) return true ;
    LOG_WARN("invalid heart code uid:%d role_id:%d heart:%d",m_uid,m_role_id,value) ;
    return false ;
}

bool ClientConnection::check_threshold(int msg_id)
{
    msgid_type mt ;
    mt.value = msg_id ;

    Threshold* th = m_threshold + mt.action_id % MAX_ACTION_SIZE ;
    th->inc() ;
    return th->is_limit() == false ;

}


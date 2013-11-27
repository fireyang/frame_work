/*
 * GateApplication.cpp
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#include <string>
#include "gate_application.h"
#include "version.h"
#include "unix_config.h"
#include "system_util.h"
#include "third_party/pugixml.hpp"
#include "message.h"
#include "common_util.h"

using namespace std;
using namespace kingnet ;
using namespace protocols::common;

int ClientAcceptor::on_connection(int fd,sa_in_t* addr)
{
    if(get_app().is_online_threshold() ) return -1 ;
    ClientConnection* conn = get_app().alloc_connection() ;
    if(conn == NULL) return -1 ;
    if( conn->init(m_reactor,fd,10240)!=0)
    {
        get_app().free_connection(conn) ;
        return -1 ;
    }
    set_socket_nodelay(fd) ;
    conn->set_readall_flag(false) ;

    char remote_addr[32] = {0} ;
    inet_ntop(AF_INET,&addr->sin_addr,remote_addr,sizeof(remote_addr)) ;
    LOG_DEBUG("client %s connected fd:%d",remote_addr,fd) ;

	conn->set_login_ip(std::string(remote_addr, strlen(remote_addr)));
    conn->on_connect();
    return 0 ;
}

GateApplication::GateApplication()
    : check_threshold(false)
	, check_seq(false)
	, check_heartbeat(false)
	, check_token(false)
    , active_heartbeat(false)
	, check_type(0)
	, token_valid_duration(0)
	, m_shut_down_delay_sec(0)
	, m_is_shuting_down(false)

{
    role_id2conn.set_empty_key(0);
    role_id2conn.set_deleted_key(-1);
}

GateApplication::~GateApplication()
{
    // TODO Auto-generated destructor stub
}

const char* GateApplication::version()
{
    return VERSION ;
}

int GateApplication::on_init()
{

    pugi::xml_document xml_config ;
    if(!xml_config.load_file(config_file))
    {
        error_return(-1,"load config failed") ;
    }
    pugi::xml_node root = xml_config.child("root") ;
    if(!root) error_return(-1,"missing <root> node") ;

    pugi::xml_node node = root.child("log") ;
    if(logger.init(node.attribute("prefix").value(),
            node.attribute("level").as_int() ) !=0 )
    {
        error_return(-1,"init logger failed") ;
    }

    node = root.child("client_acceptor") ;
    if(m_client_acceptor.init(&event_engine,
            node.attribute("listen_ip").value(),
            node.attribute("listen_port").as_int() )!=0)
    {
        error_return(-1,"init client acceptor failed") ;
    }
    set_defer_accept(m_client_acceptor.fd(),10 ) ;

    m_online_threshold = node.attribute("online_threshold").as_int() ;

	node = root.child("message_check");
	check_threshold = node.attribute("threshold").as_bool();
	check_seq = node.attribute("seq").as_bool();
	check_heartbeat = node.attribute("heartbeat").as_bool();
	active_heartbeat = node.attribute("active_heartbeat").as_bool();
	/// init token check
	node = root.child("token_check");
	check_token = node.attribute("check").as_bool();
	check_type = node.attribute("type").as_int();
	token_valid_duration = node.attribute("valid_duration").as_int();
	key1.assign(node.attribute("key1").value());
	key2.assign(node.attribute("key2").value());

	m_shut_down_delay_sec = root.child("shutdown").attribute("delay_sec").as_int();
	if (m_shut_down_delay_sec < 1 )
	{
		error_return(-1, "shutdown delay sec init failed") ;
	}

    m_server_id_list.clear(); 
	node = root.child("server_id_list");
	for (pugi::xml_node input = node.first_child(); input;
		input = input.next_sibling())
	{
		int server_id = input.attribute("id").as_int();
		m_server_id_list.insert(server_id);
	}
	if (m_server_id_list.size() == 0)
	{
		error_return(-1, "m_server_id_list init failed") ;
	}


    set_timer(60000);

	//stats_.Start();

	set_delay_stop(m_shut_down_delay_sec * 1000);

    srand(time(0));
    return 0 ;
}

int GateApplication::on_reload()
{
    m_client_acceptor.fini() ;
    //m_web_acceptor.fini() ;
    logger.fini() ;

    return on_init() ;
}

void GateApplication::on_delay_stop()
{
	if (m_is_shuting_down)
	{
		return;
	}
	m_is_shuting_down = true;


	m_client_acceptor.fini() ;
}

void GateApplication::on_fini()
{
    int counter = 0 ;
    for(ClientUIdIndexContainer::iterator it=uid2conn.begin();it!=uid2conn.end();it=uid2conn.begin())
    {
        if( (++counter) % 100 == 0 )  sleep(1) ;
		it->second->close();
    }

	for(int i = m_conn_pool.size() -1 ; i >= 0 ; --i)
    {
        delete m_conn_pool[i] ;
    }

    info_log_string(logger,"system stopped") ;
}
void GateApplication::on_timer()
{
    static int counter = 0;
    if (counter++ % 5 == 0)
    {
        //send_online_log(); // 在线日志5分钟 发一次
    }

    info_log_format(logger,"process max_time:%d max_action:%d avg_time:%d online:%d",
            m_counter.get_max().value,m_counter.get_max().key,
            m_counter.get_avg_value(),uid2conn.size() ) ;
    m_counter.clear() ;

    info_log_format(logger,"zone_connection count:%d total:%ld",zone_counter.get_count(),zone_counter.get_total() ) ;
    zone_counter.clear() ;

}

ClientConnection* GateApplication::alloc_connection()
{
    ClientConnection* conn = NULL ;
    if(m_conn_pool.size() == 0)
    {
        conn = new ClientConnection() ;
    }
    else
    {
        conn = m_conn_pool.back() ;
        m_conn_pool.pop_back() ;
    }

    return conn ;

}
void GateApplication::free_connection(ClientConnection* conn)
{
    if(m_conn_pool.size() > 100 ) delete (conn) ;
    else m_conn_pool.push_back(conn) ;

}

ClientConnection* GateApplication::get_user_connection(int32_t uid, int32_t server_id)
{
    ClientUIdIndexContainer::iterator it = uid2conn.find(std::make_pair(uid, server_id)) ;
    if( it != get_app().uid2conn.end() ) return it->second ;

    return NULL ;
}

ClientConnection* GateApplication::get_role_connection(int32_t role_id)
{
    ClientIndexContainer::iterator it = role_id2conn.find(role_id) ;
    if( it != get_app().role_id2conn.end() ) return it->second ;

    return NULL ;
}


int GateApplication::set_user_connection(int32_t uid, int32_t server_id, ClientConnection* conn)
{
    if( conn == NULL) uid2conn.erase(std::make_pair(uid, server_id)) ;
    else uid2conn.insert(std::make_pair(std::make_pair(uid, server_id), conn));
    return 0 ;
}

int GateApplication::set_role_connection(int32_t role_id,ClientConnection* conn)
{
    if( conn == NULL) role_id2conn.erase(role_id) ;
    else role_id2conn[role_id] = conn ;
    return 0 ;
}



void GateApplication::broadcast_all(const char* data,int size)
{
    for(ClientIndexContainer::iterator it=role_id2conn.begin();it!=role_id2conn.end();++it)
    {
		//get_app().stats_.AddWanTraffic(size, true);
        if(it->second->get_status() == ClientConnection::STATUS_IN_GAME) it->second->send(data,size,0) ;
    }

}

void GateApplication::stat_process_time(int32_t begin_tick,int msg_type)
{

    int process_time = (kingnet::time_ms() & 0x0fffffff) - begin_tick ;
    if( begin_tick !=0 && process_time > 0 && process_time < 5000 )
    {
        //debug_log_format(logger,"action:%d time:%d",msg_type,process_time) ;
        m_counter.add(msg_type,process_time) ;
    }



}

int GateApplication::forward_ss_packet(ClientConnection* conn,const packet_info* pi)
{
    ss_head* sshead = (ss_head*)pi->data ;
    int32_t begin_tick = ntoh_int32(sshead->tick) ;
    if( pi->type & 0x1 ) stat_process_time( begin_tick ,pi->type >> 2)  ;

    cs_head_plus head = {0};
    head.msgid = sshead->msgid ;
    head.seq = sshead->seq ;
    head.length = hton_int16(pi->size + cs_head_size(pi->type) - sizeof(ss_head) ) ;

    conn->send((const char*)&head,cs_head_size(pi->type),0) ;
    conn->send(pi->data + sizeof(ss_head),pi->size - sizeof(ss_head), 0) ;
    return 0 ;

}

int GateApplication::forward_cs_packet(const packet_info* pi,int32_t key)
{
    ss_head head = {0};
    head.key = hton_int32(key) ;
    head.seq = ((cs_head_plus*)(pi->data))->seq ;
    head.msgid = hton_int16(pi->type) ;
    head.length = hton_int16(pi->size - cs_head_size(pi->type) + sizeof(head) ) ;
    int32_t begin_tick = kingnet::time_ms() & 0x0fffffff ;
    head.tick = hton_int32(begin_tick);
    ///zone_conn.send((const char*)&head,sizeof(head),0) ;
   /// zone_conn.send(pi->data + cs_head_size(pi->type),pi->size - cs_head_size(pi->type),0) ;

    return 0 ;
}

IMPLEMENT_APPLICATION_INSTANCE(GateApplication) ;
IMPLEMENT_MAIN() ;

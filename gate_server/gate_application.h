/*
 * GateApplication.h
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#ifndef GATE_APPLICATION_H_
#define GATE_APPLICATION_H_

#include <map>
#include <set>
#include <vector>
#include <sparsehash/dense_hash_map>

#include "day_roll_logger.h"
#include "application.h"
#include "client_connection.h"
#include "tcp_acceptor.h"
#include "udp_data_handler.h"
#include "average_counter.h"

class ClientAcceptor : public kingnet::tcp_acceptor
{
public:
    int on_connection(int fd,kingnet::sa_in_t* addr) ;

};

//typedef std::map<int32_t,ClientConnection*> ClientIndexContainer ;
typedef google::dense_hash_map<int32_t,ClientConnection*> ClientIndexContainer ;
// uid, server_id => conn
typedef std::map<std::pair<int, int>, ClientConnection*> ClientUIdIndexContainer ;

class GateApplication : public kingnet::application
{
public:
	typedef struct
	{
		std::string ip ;
		int port ;
	} server_endpoint ;

public:
    GateApplication();
    virtual ~GateApplication();

    //forward ss message of server to cs message of client
    int forward_ss_packet(ClientConnection* conn,const packet_info* pi) ;
    //forward cs message of client to ss message of server
    int forward_cs_packet(const packet_info* pi,int32_t key) ;

public:

    ClientConnection* alloc_connection() ;
    void free_connection(ClientConnection* conn) ;
    ClientConnection* get_user_connection(int32_t uid, int32_t server_id) ;
    ClientConnection* get_role_connection(int32_t role_id) ;
    int set_user_connection(int32_t uid, int32_t server_id, ClientConnection* conn) ;
    int set_role_connection(int32_t role_id,ClientConnection* conn) ;
    void broadcast_all(const char* data,int size) ;
	bool is_shuting_down() { return m_is_shuting_down; }
    bool is_online_threshold() const { return m_online_threshold < (int)role_id2conn.size(); } ;

protected:
    virtual int on_init() ;
    virtual int on_reload() ;
    virtual void on_fini() ;
    virtual void on_timer() ;
    virtual const char* version()  ;
	virtual void on_delay_stop();

    void stat_process_time(int32_t begin_tick,int msg_type);

public:
    kingnet::day_roll_logger logger ;
    ClientUIdIndexContainer uid2conn ;
    ClientIndexContainer role_id2conn ;
	//GateStats stats_;
    AverageCounter zone_counter ;

	server_endpoint log_server_endpoint;

    bool check_threshold ;
    bool check_seq ;
    bool check_heartbeat ;
	bool check_token;
	bool active_heartbeat;
	int check_type;
	int token_valid_duration;
	std::string key1;
	std::string key2;

	int m_shut_down_delay_sec;

	server_endpoint& name_server_endpoint()
	{
		return m_name_server_endpoint;
	}

	bool is_server_id_valid(int server_id)
	{
		return m_server_id_list.find(server_id) != m_server_id_list.end();
	}

    bool is_server_merged()
    {
        return m_server_id_list.size() > 1 ;
    }

    const AverageCounter& get_counter() const{ return m_counter ; } ;

private:
    ClientAcceptor  m_client_acceptor ;

	bool m_is_shuting_down;
	std::set<int> m_server_id_list;

    std::vector<ClientConnection*> m_conn_pool ;

	server_endpoint m_name_server_endpoint;
    AverageCounter m_counter ;
    int32_t m_online_threshold ;
};

DECLARE_APPLICATION_INSTANCE(GateApplication) ;

#endif /* GATE_APPLICATION_H_ */

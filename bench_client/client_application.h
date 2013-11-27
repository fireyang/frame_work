/*
 * client_application.h
 *
 *  Created on: 2012-4-5
 *      Author: lixingyi
 */

#ifndef ZONE_APPLICATION_H_
#define ZONE_APPLICATION_H_

#include <string>
#include <vector>
#include "day_roll_logger.h"
#include "application.h"
#include "client_connection.h"


class client_application: public kingnet::application
{
public:
    typedef std::vector<client_connection*> client_container ;
    typedef std::vector<npc_info> npc_container ;
    typedef struct
    {
        std::string ip ;
        int port ;
    } server_endpoint ;

public:
    client_application();
    virtual ~client_application();

    const npc_info get_npc_info() ;
	const char* quest_str() {return m_quest_str;}
	const char* gm_str() {return m_gm_str;}
protected:
    virtual int on_init() ;
    virtual void on_fini() ;
    virtual void on_timer() ;

public:
    kingnet::day_roll_logger logger ;
    int sid;
private:
    server_endpoint gate_config ;
    client_container m_player_list ;
    npc_container m_npc_list ;
    client_connection* m_conn_list ;
    int m_max_count ;
    int m_begin_uid ;
    int m_cur_count ;
	char m_quest_str[1024];
	char m_gm_str[1024];

};

DECLARE_APPLICATION_INSTANCE(client_application) ;

#endif /* ZONE_APPLICATION_H_ */

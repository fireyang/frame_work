/*
 * client_application.cpp
 *
 *  Created on: 2012-4-5
 *      Author: lixingyi
 */

#include "system_util.h"
#include "client_application.h"
#include "third_party/pugixml.hpp"

using namespace kingnet ;

using namespace std;


client_application::client_application()
{
    // TODO Auto-generated constructor stub

}

client_application::~client_application()
{
    // TODO Auto-generated destructor stub
}

int client_application::on_init()
{
    pugi::xml_document xml_config ;
    if(!xml_config.load_file(config_file))
    {
        error_return(-1,"load config failed") ;
    }
    pugi::xml_node root = xml_config.child("root") ;
    if(!root) error_return(-1,"missing <root> node") ;

    pugi::xml_node node = root.child("log") ;
   
    if(logger.init(node.attribute("prefix").value() ,
        node.attribute("level").as_int() ) !=0 )
    {
        error_return(-1,"init logger failed") ;
    }

    node = root.child("gate_server") ;
    gate_config.ip.assign(node.attribute("ip").value()) ;
    gate_config.port = node.attribute("port").as_int() ;
    this->sid = node.attribute("sid").as_int();

    m_max_count = node.attribute("player_max_count").as_int() ;
    m_begin_uid = node.attribute("player_begin_uid").as_int() ;
    //m_reconnect_seconds = node.attribute("reconnect_seconds").as_int() ;
    m_cur_count = 0 ;
  
/*
	node = root.child("quest_gm_string") ;
	std::string quest_str;
	quest_str.assign(node.attribute("cmd").value());
	snprintf(m_quest_str, sizeof(m_quest_str), "%s", quest_str.c_str());

	node = root.child("pre_gm_string") ;
	std::string gm_str;
	gm_str.assign(node.attribute("cmd").value());
	snprintf(m_gm_str, sizeof(m_gm_str), "%s", gm_str.c_str());
*/

    node = root.child("rand_teleport") ;
    while(node)
    {
        npc_info npc ;
        npc.map_id = node.attribute("map_id").as_int();
        npc.map_x = node.attribute("map_x").as_int();
        npc.map_y = node.attribute("map_y").as_int();
        npc.npc_id = node.attribute("npc_id").as_int();
        m_npc_list.push_back(npc) ;

        node = node.next_sibling() ;
    }

    if(m_npc_list.size() < 1 ) error_return(-1,"no teleport info") ;
    
    m_conn_list = new client_connection[m_max_count] ;
    if(m_conn_list == NULL ) error_return(-1,"alloc player failed") ;
    srand(time(0)) ;
    set_timer(1000) ;

    return 0 ;
}

const npc_info client_application::get_npc_info()
{
    return m_npc_list[rand()% m_npc_list.size()] ;

}

void client_application::on_fini()
{
    delete[] m_conn_list ;

    info_log_string(logger,"system stopped") ;
}
void client_application::on_timer()
{
	
    if(!m_conn_list[m_cur_count].connected())
    {
        int uid = m_begin_uid + m_cur_count ;
        if(m_conn_list[m_cur_count].init(&event_engine, gate_config.ip.c_str(),gate_config.port)==0) 
        {
            
            m_conn_list[m_cur_count].run(uid,1000) ;
        }
    }

    m_cur_count = (m_cur_count +1) % m_max_count ;
	
       
}


IMPLEMENT_APPLICATION_INSTANCE(client_application) ;
IMPLEMENT_MAIN() ;



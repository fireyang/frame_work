/*
 * fsm_manager.cpp
 *
 *  Created on: 2011-12-15
 *      Author: Administrator
 */


#include <stddef.h>
#include "fsm_manager.h"

namespace kingnet
{

fsm_manager::fsm_manager()
{
    m_seq = 0 ;
    //m_container.set_empty_key(-1);
    //m_container.set_deleted_key(-2);
}


fsm_manager::~fsm_manager()
{

}

void fsm_manager::clear()
{
    for(fsm_container::iterator it = m_container.begin();it!=m_container.end();++it)
    {
        free_fsm(it->second) ;
    }

    m_container.clear() ;
}

base_fsm* fsm_manager::get_fsm(int fsm_id)
{
    fsm_container::iterator it = m_container.find(fsm_id) ;
    if(it != m_container.end() ) return it->second ;

    return NULL ;
}


base_fsm* fsm_manager::create_fsm(int fsm_type)
{
    base_fsm* object = alloc_fsm(fsm_type) ;
    if(object == NULL) return NULL ;

    m_seq = (m_seq + 1) & FSM_ID_MASK ;
    object->m_id = m_seq ;

    m_container[object->m_id] = object ;

    return object ;
}

void fsm_manager::destroy_fsm(base_fsm* object)
{
    fsm_container::iterator it = m_container.find(object->m_id) ;
    if(it != m_container.end())
    {
        m_container.erase(it) ;
        free_fsm(object) ;
    }
}




}

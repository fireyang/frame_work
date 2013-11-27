/*
 * fsm_manager.h
 *
 *  Created on: 2011-12-15
 *      Author: Administrator
 */

#ifndef FSM_MANAGER_H_
#define FSM_MANAGER_H_

#include <stdint.h>
#include <map>

namespace kingnet
{

class fsm_manager ;

class base_fsm
{
friend class fsm_manager ;
public:
    base_fsm():m_id(0) { } ;
    virtual ~base_fsm() { } ;

    /*
     * @brief fsm entry , callback implemented by concrete class
     * @param [in] fsm manager
     * @param [in] event type
     * @param [in] arg
     *
     */
    virtual void enter(fsm_manager* fm , int event_type , void* arg) = 0 ;

public:
    /*
     * @brief get fsm id assigned by fsm manager
     */
    int32_t get_id() const { return m_id ; } ;

private:
    int32_t m_id ;
};

class fsm_manager
{
public:
    typedef std::map<int32_t,base_fsm*> fsm_container ;
    //typedef google::dense_hash_map<int32_t,base_fsm*> fsm_container ;
    enum
    {
        FSM_ID_MASK = 0xffffff ,
    };
public:
    fsm_manager() ;
    virtual ~fsm_manager() ;

    /*
     * @brief create fsm object by type which owned by fsm_manager
     * @param [in] fsm type
     * @return fsm object created , NULL on failed
     */
    base_fsm* create_fsm(int fsm_type)  ;

    /*
     * @brief destroy fsm
     * @param [in] object to be destroyed
     */
    void destroy_fsm(base_fsm* object) ;

    /*
     *@brief get fsm object by fsm_id
     *@param [in] fsm id
     *@return fsm object
     */
    base_fsm* get_fsm(int fsm_id) ;

    /*
     * @brief clean up all fsm object it hold
     */
    void clear() ;

    /*
     * @brief get fsm count
     */
    int size() const { return m_container.size() ; } ;

protected:
    /*
     * @brief alloc fsm memory, implemented by concrete class
     * @return fsm object pointer
     */
    virtual base_fsm* alloc_fsm(int type) = 0 ;

    /*
     * @brief free fsm memory, implemented by concrete class
     */
    virtual void free_fsm(base_fsm* object)  = 0 ;

private:
    fsm_manager(const fsm_manager&) ;
    fsm_manager& operator=(const fsm_manager&) ;

private:
    fsm_container m_container ;
    int32_t m_seq ;
};


}

#endif /* FSM_MANAGER_H_ */

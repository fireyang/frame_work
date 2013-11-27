
#ifndef LRU_MAP_CONTAINER_H_
#define LRU_MAP_CONTAINER_H_

#include <map>
#include <tr1/unordered_map>
//#include "sparsehash/dense_hash_map"


template <typename T>
class LRUMapContainer
{
public:
    class node_type
    { 
    friend class LRUMapContainer ;
    public:
        T& value() { return m_value ;} ;
    private:
        node_type* m_prev ;
        node_type* m_next ;
        T m_value ;
    } ;

    typedef int key_type ;
    //typedef std::map<key_type,node_type > node_container ;
    typedef std::tr1::unordered_map<key_type,node_type > node_container ;

    typedef typename node_container::iterator iterator ;
    typedef typename node_container::const_iterator const_iterator ;

public:
    iterator begin() { return m_container.begin() ; } ;
    const_iterator begin() const { return m_container.begin() ; } ;

	int  size() const { return m_container.size(); } ;

    iterator end() { return m_container.end() ; } ;
    const_iterator end() const { return m_container.end() ; } ;
    
public:
    LRUMapContainer()
    {
        m_head.m_next = m_head.m_prev = &m_head ;
        //m_container.set_deleted_key(-1) ;
        //m_container.set_empty_key(-2) ;
    };

    iterator get_node(int key,bool lru_touch=false) 
    {
        iterator it = m_container.find(key) ;
        if(it != m_container.end() && lru_touch )
        {
            list_node_remove(&it->second) ;
            list_node_insert(&m_head,&it->second,m_head.m_next) ;
        } 
        
        return it ;
    }
    
    T* get(int key,bool lru_touch=false)
    {
        iterator it = get_node(key,lru_touch) ;
        if(it!= m_container.end() ) return &it->second.m_value ;
        return NULL ;
    }

    T* add(int key)
    {
        iterator it = m_container.find(key);
        if(it == m_container.end())
        {
            node_type& node = m_container[key] ;
            //node.m_value = value ;
            list_node_insert(&m_head,&node,m_head.m_next) ;
            return &node.m_value ;
        }
        
        return NULL ;
    }

    T* add(int key,const T& value)
    {
        iterator it = m_container.find(key);
        if(it == m_container.end())
        {
            node_type& node = m_container[key] ;
            node.m_value = value ;
            list_node_insert(&m_head,&node,m_head.m_next) ;
            return &node.m_value ;
        }
        
        return NULL ;
    }

    void remove(int key) 
    {
        iterator it = m_container.find(key) ;
        if(it != m_container.end() ) remove(it);
        
    }

    void remove(iterator it)
    {
        node_type& node = it->second ;
        list_node_remove(&node) ;
        m_container.erase(it) ;        
    }

    void clear()
    {
        m_head.m_next = m_head.m_prev = &m_head ;
        m_container.clear() ;
    }
    
    T* lru_tail()
    {
        if(&m_head == m_head.m_prev) return NULL ;
        return &m_head.m_prev->m_value ;
    }
    
    T* lru_head()
    {
        if(&m_head == m_head.m_next) return NULL ;
        return &m_head.m_next->m_value ;    
    }

    node_type* lru_next(node_type* node)
    {
        if ( node == NULL ) node = m_head ;
        if( node->m_next != &m_head )
        {
            return node->m_next ;
        }

        return NULL ;
    }

private:
    void list_node_insert(node_type* prev,node_type* curr,node_type* next)
    {
        prev->m_next = curr ;

        curr->m_prev = prev ;
        curr->m_next = next ;

        next->m_prev = curr ;
    }

    void list_node_remove(node_type* node)
    {
        node->m_prev->m_next = node->m_next ;
        node->m_next->m_prev = node->m_prev ;

        node->m_next = node->m_prev = NULL ;
    }

private:
    node_container m_container ;
    node_type m_head ;

} ;


#endif



/*
 * object_pool.h
 *
 *  Created on: 2012-4-12
 *      Author: lixingyi
 */

#ifndef OBJECT_POOL_H_
#define OBJECT_POOL_H_

#include <stdint.h>
#include <stdlib.h>
#include <map>
#include <tr1/unordered_map>

template<typename T>
class object_pool
{
public:
    typedef T object_type ;
    typedef int64_t key_type ;
    //typedef std::map<key_type,object_type*> object_container ;
    typedef std::tr1::unordered_map<key_type,object_type*> object_container ;
	typedef typename object_container::size_type   size_type;
    typedef typename object_container::iterator iterator ;
    typedef typename object_container::const_iterator const_iterator ;

public:
    iterator begin() { return m_objects.begin() ; } ;
    const_iterator begin() const { return m_objects.begin() ; } ;

	size_type  size() const { return m_objects.size(); } ;

    iterator end() { return m_objects.end() ; } ;
    const_iterator end() const { return m_objects.end() ; } ;
    
public:
    object_pool()
    {

    }

    ~object_pool()
    {
        clear_with_delete();
    }

    void clear_with_delete()
    {
        for(typename object_container::iterator it=m_objects.begin();it!=m_objects.end();++it)
        {
			if (it->second) {
				delete it->second ;
				it->second = NULL;
			}
        }

        m_objects.clear() ;
    }

	void clear()
	{
		m_objects.clear() ;
	}

    object_type* create(key_type key)
    {
        typename object_container::iterator it = m_objects.find(key) ;
        if(it == m_objects.end() )
        {
            object_type* obj = new object_type() ;
            if(obj == NULL) return NULL ;
            m_objects[key] = obj ;
            return obj ;
        }

        return NULL ;
    }

    int add(key_type key,object_type* obj)
    {
        typename object_container::iterator it = m_objects.find(key) ;
        if(it == m_objects.end() )
        {
            
            m_objects[key] = obj ;
            return 0 ;
        }

        return -1 ;        
    }


    void destroy(key_type key)
    {
        typename object_container::iterator it = m_objects.find(key) ;
        if(it != m_objects.end() )
        {
            delete it->second ;
            m_objects.erase(it) ;
        }
    }

	void erase(key_type key)
	{
		typename object_container::iterator it = m_objects.find(key) ;
        if(it != m_objects.end() )
        {
            m_objects.erase(it) ;
        }
	}

	void erase ( iterator first, iterator last )
	{
		if (first != m_objects.end())
		{
			m_objects.erase(first, last) ;
		}
	}

    object_type* get(key_type key)
    {
        typename object_container::iterator it = m_objects.find(key) ;
        if(it != m_objects.end() ) return it->second ;
        return NULL ;
    }

	const object_type* get(key_type key) const
	{
		typename object_container::const_iterator it = m_objects.find(key) ;
		if(it != m_objects.end() ) return it->second ;
		return NULL ;
	}

private:
    object_container m_objects ;
};


#endif /* OBJECT_POOL_H_ */

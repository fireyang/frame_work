/*
 * atomic_int.h
 *
 *  Created on: 2011-10-14
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef ATOMIC_INT_H_
#define ATOMIC_INT_H_

#include <stdint.h>

namespace kingnet
{

template<typename T>
class atomic_int
{
public:
    typedef T int_type ;

    atomic_int():m_counter(0) { } ;
    ~atomic_int() { } ;


    /*
     * brief atomically read value
     */
    int_type get() { return m_counter ; } ;

    /*
     * @brief atomically set value
     */
    int_type set(int_type v) { m_counter = v ; } ;

    /**
     * @brief atomically add a value and return new value
     * @param [in]: integer value to add
     * @return new value
     */
    int_type add(int_type i) { return __sync_add_and_fetch(&m_counter,i) ; } ;

    /**
     * @brief atomically sub a value and return new value
     * @param [in]: integer value to sub
     * @return new value
     */
    int_type sub(int_type i) { return __sync_sub_and_fetch(&m_counter,i) ; } ;


private:
    volatile int_type m_counter ;

};

typedef atomic_int<int8_t> atomic_int8 ;
typedef atomic_int<int16_t> atomic_int16 ;
typedef atomic_int<int32_t> atomic_int32 ;
typedef atomic_int<int64_t> atomic_int64 ;


}

#endif /* ATOMIC_INT_H_ */

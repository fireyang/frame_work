
#ifndef AVERAGE_COUNTER_H_
#define AVERAGE_COUNTER_H_

class AverageCounter 
{
public:
    typedef struct
    {
        int key ;
        int value ;
    } Counter ;

public:
    AverageCounter() { clear() ; } ;
    void clear() 
    {
        m_total = m_count = 0 ;
        m_min.key = m_min.value = 99999999 ;
        m_max.key = m_max.value = 0 ;
    }

    void add(int key,int value)
    {
        m_total += value ;
        m_count += 1 ;

        if( value > m_max.value ) 
        {
            m_max.value = value ;
            m_max.key = key ;
        }

        if( value < m_min.value ) 
        {
            m_min.value = value ;
            m_min.key = key ;
        }

    }
 
    Counter get_min() const { return m_min ; } ;
    Counter get_max() const{ return m_max ; } ;
    int get_avg_value() const{ return m_count==0?0:m_total/m_count ; } ;
    int64_t get_total() const{ return m_total ; } ;
    int64_t get_count() const{ return m_count; } ;

private:
    int64_t m_total ;
    int64_t m_count ;
    Counter m_min ;
    Counter m_max ;


} ;

#endif


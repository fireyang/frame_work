#ifndef THRESHOLD_H_
#define THRESHOLD_H_

class Threshold
{
public:
    void init(int cycle_second,int limit)
    {
        if(cycle_second < 1 ) cycle_second = 1 ;
        if(limit < 1 ) limit = 1 ;

        m_cycle_second = cycle_second ;
        m_limit = limit ;
        m_cur_cycle = 0 ;
        m_cur_counter = 0 ;
    }

    void inc()
    {
        int now_cycle = time(0)/m_cycle_second ;
        if(now_cycle == m_cur_cycle)
        {
            m_cur_counter +=1 ;
        }
        else
        {
            m_cur_cycle = now_cycle ;
            m_cur_counter = 1 ;
        }

    }

    bool is_limit()
    {
        return m_cur_counter > m_limit ;
    }


private:
    int m_cur_cycle ;
    int m_cur_counter ;
    int m_limit ;
    int m_cycle_second ;
} ;

#endif



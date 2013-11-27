#ifndef RWCONTAINER_H_
#define RWCONTAINER_H_

template <typename T>
class RWContainer
{
public:
    RWContainer():m_read(NULL) { } ;
        
    void transit()
    {
        if(m_read == &m_data_a ) m_read = &m_data_b ;
        else m_read = &m_data_a ;
    }

    T* readable() { return m_read ;} ;

    T* writable() { return  m_read == &m_data_a ? &m_data_b : &m_data_a ; } ;

private:

    T m_data_a ;
    T m_data_b ;
    T* m_read ;

} ;

#endif


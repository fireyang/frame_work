#include "id_counter.h" 

id_counter::id_counter()
{
    tid = 0;
    sid = 0;
    counter = 0;
    step = 0;
    memset(dirty,1,sizeof(dirty));
}
void id_counter::load(const char** data)
{
    tid = (int32_t)atoi(data[0]);
    sid = (int32_t)atoi(data[1]);
    counter = (int32_t)atoi(data[2]);
    step = (int32_t)atoi(data[3]);
    remark.assign(data[4]);
    memset(dirty,0,sizeof(dirty));
}
void id_counter::load(const vector<string>& data)
{
    tid = (int32_t)atoi(data[0].c_str());
    sid = (int32_t)atoi(data[1].c_str());
    counter = (int32_t)atoi(data[2].c_str());
    step = (int32_t)atoi(data[3].c_str());
    remark.assign(data[4].c_str());
    memset(dirty,0,sizeof(dirty));
}
int id_counter::sql_key(char* buf,int size) const
{
    int origin_size = size ,len = 0; 
    len =sql_tid(buf,size);
    buf += len ; size -= len ;
    memcpy(buf," and ",5); buf+= 5 ; size-= 5 ;
    len =sql_sid(buf,size);
    buf += len ; size -= len ;
    return origin_size - size ; 
} 
int id_counter::sql_data(char* buf,int size,bool check_dirty) const
{
    int origin_size = size,len=0 ; 
    int first_flag = 1 ; 
    if((!check_dirty) || dirty[0])
    {
        if(first_flag==0) {memcpy(buf,",",1); buf+= 1 ; size-= 1 ;}
        len =sql_tid(buf,size);
        buf += len ; size -= len ;first_flag=0;
    }
    if((!check_dirty) || dirty[1])
    {
        if(first_flag==0) {memcpy(buf,",",1); buf+= 1 ; size-= 1 ;}
        len =sql_sid(buf,size);
        buf += len ; size -= len ;first_flag=0;
    }
    if((!check_dirty) || dirty[2])
    {
        if(first_flag==0) {memcpy(buf,",",1); buf+= 1 ; size-= 1 ;}
        len =sql_counter(buf,size);
        buf += len ; size -= len ;first_flag=0;
    }
    if((!check_dirty) || dirty[3])
    {
        if(first_flag==0) {memcpy(buf,",",1); buf+= 1 ; size-= 1 ;}
        len =sql_step(buf,size);
        buf += len ; size -= len ;first_flag=0;
    }
    if((!check_dirty) || dirty[4])
    {
        if(first_flag==0) {memcpy(buf,",",1); buf+= 1 ; size-= 1 ;}
        len =sql_remark(buf,size);
        buf += len ; size -= len ;first_flag=0;
    }
    return origin_size - size ; 
} 
int id_counter::sql_query(char* buf,int size) const
{
    int origin_size = size ,len=0; 
    len = snprintf(buf,size,"select * from id_counter where ");
    buf += len ; size -= len ;
    len = sql_key(buf,size) ;
    buf += len ; size -= len ;
    return origin_size - size ; 
} 
int id_counter::sql_insert(char* buf,int size) const
{
    int origin_size = size,len=0 ; 
    len = snprintf(buf,size,"insert into id_counter set ");
    buf += len ; size -= len ;
    len = sql_data(buf,size,false) ;
    buf += len ; size -= len ;
    return origin_size - size ; 
} 
int id_counter::sql_replace(char* buf,int size) const
{
    int origin_size = size,len=0 ; 
    len = snprintf(buf,size,"replace into id_counter set ");
    buf += len ; size -= len ;
    len = sql_data(buf,size,false) ;
    buf += len ; size -= len ;
    return origin_size - size ; 
} 
int id_counter::sql_update(char* buf,int size) const
{
    int origin_size = size,len=0 ; 
    len = snprintf(buf,size,"update id_counter set ");
    buf += len ; size -= len ;
    len = sql_data(buf,size,true) ;
    buf += len ; size -= len ;
    memcpy(buf," where ",7); buf+= 7 ; size-= 7 ;
    len = sql_key(buf,size) ;
    buf += len ; size -= len ;
    return origin_size - size ; 
} 
int id_counter::sql_delete(char* buf,int size) const
{
    int origin_size = size ,len=0; 
    len = snprintf(buf,size,"delete from id_counter where ");
    buf += len ; size -= len ;
    len = sql_key(buf,size) ;
    buf += len ; size -= len ;
    return origin_size - size ; 
} 

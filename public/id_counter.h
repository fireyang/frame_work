#ifndef _generate_h_id_counter 
#define _generate_h_id_counter 
#include "sql_binder.h"
class id_counter : public sql_binder 
{
public:
    id_counter();
    void clear_dirty(){ memset(dirty,0,sizeof(dirty)); } ;
    bool is_dirty() const
    {
        for(int i=0;i<FIELD_COUNT;++i) {if(dirty[i]) return true;} ;
        return false ;
    } ;
    void load(const char** data);
    void load(const vector<string>& data);
    int sql_insert(char* buf,int size) const;
    int sql_replace(char* buf,int size) const;
    int sql_update(char* buf,int size) const;
    int sql_query(char* buf,int size) const;
    int sql_delete(char* buf,int size) const;
protected:
    int sql_key(char* buf,int size) const;
    int sql_data(char* buf,int size,bool check_dirty=true) const;
public:
    int32_t get_tid() const { return tid;} ; 
    void set_tid(int32_t value) { if(tid!= value){dirty[0] = 1; tid = value;} } ; 
    int sql_tid(char* buf,int size) const{return snprintf(buf,size,"tid='%ld'",(int64_t)tid);}

    int32_t get_sid() const { return sid;} ; 
    void set_sid(int32_t value) { if(sid!= value){dirty[1] = 1; sid = value;} } ; 
    int sql_sid(char* buf,int size) const{return snprintf(buf,size,"sid='%ld'",(int64_t)sid);}

    int32_t get_counter() const { return counter;} ; 
    void set_counter(int32_t value) { if(counter!= value){dirty[2] = 1; counter = value;} } ; 
    int sql_counter(char* buf,int size) const{return snprintf(buf,size,"counter='%ld'",(int64_t)counter);}

    int32_t get_step() const { return step;} ; 
    void set_step(int32_t value) { if(step!= value){dirty[3] = 1; step = value;} } ; 
    int sql_step(char* buf,int size) const{return snprintf(buf,size,"step='%ld'",(int64_t)step);}

    const string& get_remark() const { return remark;} ; 
    void set_remark(const string& value) { if(remark!= value){dirty[4] = 1; remark.assign(value);} }; 
    void set_remark(const char* value) { if(strcmp(remark.c_str(),value)!=0) {dirty[4] = 1; remark.assign(value);} }; 
    int sql_remark(char* buf,int size) const{return snprintf(buf,size,"remark='%s'",remark.c_str());}

private:
    //data member
    int32_t tid ; 
    int32_t sid ; 
    int32_t counter ; 
    int32_t step ; 
    string remark ; 
private:
    //dirty flag for update
    enum { FIELD_COUNT = 5 } ; 
    int8_t dirty[FIELD_COUNT] ; 
};
#endif

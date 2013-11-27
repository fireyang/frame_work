/*
 * game_packet.h
 *
 *  Created on: 2012-4-1
 *      Author: lixingyi
 */

#ifndef GAME_PACKET_H_
#define GAME_PACKET_H_

#include <sstream>
#include <exception>
#include "packet.h"
#include "aliased_buf.h"
#include "game_common.pb.h"


struct ss_head
{
    uint16_t length ;
    uint16_t msgid ;
    uint32_t key ;
    uint32_t seq ;
	uint32_t tick;
} __attribute__((packed)) ;

struct cs_head
{
    uint16_t length ;
    uint16_t msgid ;
} __attribute__((packed)) ;

struct cs_head_plus
{
    uint16_t length ;
    uint16_t msgid ;
    uint16_t code ;
    uint16_t reserved ;
    uint32_t seq ;
} __attribute__((packed)) ;

union msgid_type
{
    uint16_t value ;
    struct
    {
        uint16_t type:2 ;
        uint16_t action_id:10 ;
        uint16_t module:4 ;

    } __attribute__((packed)) ;
} __attribute__((packed)) ;

union position_type
{
    uint64_t value ;
    struct
    {
        uint32_t map_id ;
        uint16_t x ;
        uint16_t y ;
    };
} __attribute__((packed)) ;


template<int32_t PT,typename BT>
class ss_packet : public kingnet::packet
{
public:
    enum{ packet_type = PT ,} ;
    typedef BT body_type ;
public:
	ss_packet()
		: tick(0)
	{
	}
    virtual int get_type() {return packet_type ; } ;

    virtual int encode(char* data,int size)
    {
        if(size < (int)sizeof(ss_head) ) return -1 ;
        aliased_buf sbuf(data +sizeof(ss_head), size - sizeof(ss_head)) ;
        std::ostream output(&sbuf) ;
        try
        {
            if(!body.SerializeToOstream(&output) || output.tellp() < 0 ) return -1 ;
        }
        catch(const std::exception& ex)
        {
            return -1 ;
        }
        
        size = sizeof(ss_head) + output.tellp() ;
        if(size >= 65535 ) return -1 ;
        ss_head* head = (ss_head*)data ;
        head->length = hton_int16(size) ;
        head->msgid = hton_int16(PT) ;
        head->key = hton_int32(key) ;
        head->seq = hton_int32(seq) ;
		head->tick = hton_int32(tick);
        return size ;

    }
    virtual int decode(const char* data,int size)
    {
        if(size < (int)sizeof(ss_head) ) return -1 ;

        ss_head* head = (ss_head*)data ;
        if(ntoh_int16(head->msgid)!= PT) return -1 ;
        key = ntoh_int32(head->key) ;
        seq = ntoh_int32(head->seq) ;
		tick = ntoh_int32(head->tick);
        size = ntoh_int16(head->length) ;
        
        aliased_buf sbuf((char*)data +sizeof(ss_head), size - sizeof(ss_head)) ;

        std::istream input(&sbuf) ;
        try
        {
            if(!body.ParseFromIstream(&input) ) return -1 ;
        }
        catch(const std::exception& ex)
        {
            return -1 ;
        }
        
        return size ;

    }

    virtual int encode_size()
    {
        int need_size = (int)sizeof(ss_head) + body.ByteSize() ;
        if( need_size >= 65535 ) return -1 ;
        return need_size ;
    }

    virtual int decode_size(const char* data,int size)
    {
        ss_head* tmp = (ss_head*)data ;
        return  ntoh_int16(tmp->length) ;

    }

public:
    uint32_t key ;
    uint32_t seq ;
	uint32_t tick;
    body_type body ;
};

inline bool cs_head_have_plus(uint16_t msgid)
{
    return  (msgid & 0x3) != protocols::common::MSG_TYPE_NOTIFY ;
}

inline int cs_head_size(uint16_t msgid)
{
    if(cs_head_have_plus(msgid)) return (int)sizeof(cs_head_plus) ;
    return (int)sizeof(cs_head) ;
}

template<int32_t PT,typename BT>
class cs_packet : public kingnet::packet
{
public:
    enum{ packet_type = PT ,} ;
    typedef BT body_type ;
public:


    virtual int get_type() {return packet_type ; } ;

    virtual int encode(char* data,int size)
    {
        if(size < cs_head_size(PT) ) return -1 ;

        aliased_buf sbuf(data +cs_head_size(PT), size - cs_head_size(PT)) ;
        std::ostream output(&sbuf) ;
        try
        {
            if(!body.SerializeToOstream(&output) || output.tellp() < 0 ) return -1 ;
        }
        catch(const std::exception& ex)
        {
            return -1 ;
        }
        
        size = cs_head_size(PT) + output.tellp() ;
        if(size >= 65535 ) return -1 ;
        cs_head_plus* head = (cs_head_plus*)data ;
        head->length = hton_int16(size) ;
        head->msgid = hton_int16(PT) ;
        if(cs_head_have_plus(PT))
        {
            head->code = hton_int16(code) ;
            head->reserved = hton_int16(reserved) ;
            head->seq = hton_int32(seq) ;
			//head->tick = hton_int32(tick) ;
        }

        return size ;

    }
    virtual int decode(const char* data,int size)
    {
        if(size < cs_head_size(PT) ) return -1 ;

        cs_head_plus* head = (cs_head_plus*)data ;
        if(ntoh_int16(head->msgid)!= PT) return -1 ;


        size = ntoh_int16(head->length) ;
        if(cs_head_have_plus(PT))
        {
            code = ntoh_int16(head->code) ;
            reserved = ntoh_int16(head->reserved) ;
            seq = ntoh_int32(head->seq) ;
			//tick = ntoh_int32(head->tick);
        }

        aliased_buf sbuf((char*)data +cs_head_size(PT), size - cs_head_size(PT)) ;
        std::istream input(&sbuf) ;
        try
        {
            if(!body.ParseFromIstream(&input) ) return -1 ;
        }
        catch(const std::exception& ex)
        {
            return -1 ;
        }

        return size ;

    }

    virtual int encode_size()
    {
        int need_size = cs_head_size(PT) + body.ByteSize() ;
        if(need_size >= 65535 )  return -1 ;
        return need_size ;
    }

    virtual int decode_size(const char* data,int size)
    {
        cs_head_plus* tmp = (cs_head_plus*)data ;
        return  ntoh_int16(tmp->length) ;

    }

public:
    int16_t code ;
    int16_t reserved ;
    int32_t seq ;
    body_type body ;
};

template<int32_t PT,typename BT>
class ss_codecs : public kingnet::packet
{
public:
    enum{ packet_type = PT ,} ;
    typedef BT body_type ;
public:
    virtual int get_type() {return packet_type ; } ;

    virtual int encode(char* data,int size)
    {
        if(size < sizeof(ss_head) ) return -1 ;
        aliased_buf sbuf(data +sizeof(ss_head), size - sizeof(ss_head)) ;
        std::ostream output(&sbuf) ;
        if(!body->SerializeToOstream(&output) || output.tellp() < 0 ) return -1 ;
        size = sizeof(ss_head) + output.tellp() ;
        if(size >= 65535 ) return -1 ;
        ss_head* head = (ss_head*)data ;
        head->length = hton_int16(size) ;
        head->msgid = hton_int16(PT) ;
        head->key = hton_int32(key) ;
        head->seq = hton_int32(seq) ;

        return size ;

    }
    virtual int decode(const char* data,int size)
    {
        if(size < sizeof(ss_head) ) return -1 ;

        ss_head* head = (ss_head*)data ;
        if(ntoh_int16(head->msgid)!= PT) return -1 ;
        key = ntoh_int32(head->key) ;
        seq = ntoh_int32(head->seq) ;

        size = ntoh_int16(head->length) ;
        
        aliased_buf sbuf((char*)data +sizeof(ss_head), size - sizeof(ss_head)) ;
        std::istream input(&sbuf) ;
        if(!body->ParseFromIstream(&input) ) return -1 ;
        
        return size ;

    }

    virtual int encode_size()
    {
        return sizeof(ss_head) + body->ByteSize() ;
    }

    virtual int decode_size(const char* data,int size)
    {
        ss_head* tmp = (ss_head*)data ;
        return  ntoh_int16(tmp->length) ;

    }

public:
    uint32_t key ;
    uint32_t seq ;
    body_type* body ;
};


class null_body
{
public:
    static bool  ParseFromIstream(std::istream* input) { return true ; } ;
    static bool  SerializeToOstream(std::ostream* output) { return true ; } ;
    static int ByteSize() { return 0 ; } ;
    static int encode(char* data,int size) { return 0 ;} ;
    static int decode(const char* data,int size) { return 0 ; } ;
    static int encode_size() {return 0 ; } ;
}  ;


#endif /* GAME_PACKET_H_ */

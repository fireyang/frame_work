/*
 *  * message.h
 *   *
 *    *  Created on: 2012-4-1
 *     *      Author: lixingyi
 *      */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <sstream>

#include "game_packet.h"
#include "game_client.pb.h"
using namespace protocols::common ;

enum
{
    CSMSG_PLAYER_LOGIN_REQ = MSG_ACTION_PLAYER_LOGIN<<2|MSG_TYPE_REQUEST,
    CSMSG_PLAYER_LOGIN_REP = MSG_ACTION_PLAYER_LOGIN<<2|MSG_TYPE_RESPONSE,
};

enum
{
    //SSMSG_BROADCAST = MSG_MODULE_SERVER<< 12 | MSG_ACTION_BROADCAST << 2 | MSG_TYPE_NOTIFY ,
    //SSMSG_SYNC_POSITION =MSG_MODULE_SERVER<< 12 | MSG_ACTION_SYNC_POSITION << 2 | MSG_TYPE_NOTIFY ,
} ;

typedef cs_packet<CSMSG_PLAYER_LOGIN_REQ, PlayerLoginRequest> cs_player_login_request;
typedef cs_packet<CSMSG_PLAYER_LOGIN_REQ, PlayerLoginResponse> cs_player_login_response;

#endif /* MESSAGE_H_ */


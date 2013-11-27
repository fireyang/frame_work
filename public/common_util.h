/*
 * common_util.h
 *
 *  Created on: 2012-9-2
 *      Author: lixingyi
 */

#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <stdint.h>
#include <map>
#include <set>
#include <vector>

enum {
	DEFAULT_MAP_ID = 7,
	DEFAULT_MAP_X = 14,
	DEFAULT_MAP_Y = 28,

    GUILD_WAR_PVP_MAP_ID = 53,
};

#define PLAYER_GUID(TID,ROLE_ID)  (((int64_t)(TID) << 32) | (ROLE_ID))
#define PLAYER_ROLE_ID(GUID) ((GUID)&0xffffffff)
#define PLAYER_UID(GUID) ((GUID)&0xffffffff)
#define PLAYER_TID(GUID) ((GUID) >> 32 )

#define ITEM_GUID(TID,SEQ)  (((int64_t)(TID) << 32) | (SEQ))
#define ITEM_TID(GUID) ((GUID) >> 32 )

typedef std::set<int>  IntIdSet ;
typedef std::vector<int> IntIdArray;

typedef IntIdSet       IdContainer ;

typedef std::pair<int, int>  IntPair; 
typedef std::map<int, int>  IntMap;

typedef std::vector<IntPair>   IntPairVector;

// sort map by value , lesser is front
int map_value_less(const std::pair<int, int>& left, const std::pair<int, int>& right);

// sort map by value, greater is front
int map_value_greater(const std::pair<int, int>& left, const std::pair<int, int>& right);


#endif /* */


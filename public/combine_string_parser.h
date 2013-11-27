/*******************************************************************************
 *
 *      @file: combine_string_parser.h
 *
 *      @brief: 组合字符串( 1:2|3:1|2:1 ) 的解析
 *
 *      @author: gxl. 2012-5-23
 *
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/
#ifndef COMBINE_STRING_PARSER_H
#define COMBINE_STRING_PARSER_H

#include <string>
#include <vector>
#include <map>

class combine_string_parser
{
public:
    combine_string_parser() : orig_str("") {}
    ~combine_string_parser() {}

    void parse_string(const std::string&  comb_str, bool is_key_value=true); 

    static void split_by(const std::string& orig, char seperator, 
        std::vector<std::string>&  raw_strings);

	// format x:x:x:x|y:y:y:y|z:z:z:z
	std::string  serialize_item_list_to_string(); 

    typedef  std::vector<int>   Item_Detail_Property;
    typedef  std::vector<Item_Detail_Property>   Item_Container;

    typedef  std::map<int, int>  Item_Dict;

public:
    std::string     orig_str;
    Item_Container  item_list;    
    Item_Dict       item_map;

};











#endif /* COMBINE_STRING_PARSER_H */


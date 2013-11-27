/*******************************************************************************
 *
 *      @file: combine_string_parser.cpp
 *
 *      @brief: 组合字符串( 1:2|3:1|2:1 ) 的解析
 *
 *      @author: gxl. 2012-5-23
 *
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/
#include "combine_string_parser.h"
#include <sstream>
#include <stdlib.h>

using namespace std;

void combine_string_parser::split_by(const std::string& orig, char seperator, 
              std::vector<std::string>&  raw_strings)
{
    if (orig.size() == 0)  return ;

    string::size_type  pos = 0;
    string::size_type  flag_pos = orig.find_first_of(seperator);

    while (flag_pos != string::npos) 
    {
        raw_strings.push_back(orig.substr(pos, flag_pos-pos));
        pos = flag_pos + 1;

        flag_pos = orig.find_first_of(seperator, pos);
    }
    raw_strings.push_back(orig.substr(pos));

}


void combine_string_parser::parse_string(const std::string& comb_str, bool is_key_value)
{
    if (comb_str.size() == 0) return;
    
	orig_str.clear();
	item_list.clear();
	item_map.clear();

    orig_str.assign(comb_str);

    vector<string>  raw_strings;
    combine_string_parser::split_by(orig_str, '|', raw_strings);

    vector<string>::iterator  itor = raw_strings.begin();
    for( ; itor != raw_strings.end(); ++itor)
    {
        vector<string>  sec_raw_strings;
        Item_Detail_Property  item_detail;

        combine_string_parser::split_by(*itor, ':', sec_raw_strings);
        if (sec_raw_strings.size() > 0)
        {
            for(vector<string>::iterator  itor = sec_raw_strings.begin();
                itor != sec_raw_strings.end(); ++itor)
            {
                item_detail.push_back(atoi((*itor).c_str()));
            }
            item_list.push_back(item_detail);
        }
    }

    if (is_key_value)
    {
        for (size_t i = 0; i < item_list.size(); i++)
        {
            Item_Detail_Property&   item_detail = item_list[i];
            if (item_detail.size() == 2)
            {
                item_map.insert( make_pair(item_detail[0], item_detail[1]) );
            }
        }
    }
}


std::string  combine_string_parser::serialize_item_list_to_string()
{
	string  tmp_string = "";
	ostringstream out;

	if (item_list.size() > 1)
	{
		for (size_t i=0; i < item_list.size() -1; i++)
		{
			Item_Detail_Property&  item_detail = item_list[i];
			for (size_t index=0; index < item_detail.size() -1; index++)
			{
				out << item_detail[index] << ":" ;
			}
			if (item_detail.size() >= 1)
			{
				out << item_detail[ item_detail.size()-1 ] ;
			}
			
			out << "|" ;
		}

		if (item_list.size() -1 >= 0)
		{
			Item_Detail_Property&  item_detail = item_list[item_list.size() -1];
			for (size_t index=0; index < item_detail.size() -1; index++)
			{
				out << item_detail[index] << ":" ;
			}
			if (item_detail.size() >= 1)
			{
				out << item_detail[ item_detail.size()-1 ] ;
			}
		}
		
	}
	
	tmp_string = out.str();

	return tmp_string;
}















#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "str_util.h"

using namespace std;

/* 
  All kinds of space 
    "\0" - NULL
    "\t" - tab
    "\n" - new line
    "\x0B" - 纵向列表符
    "\r" - 回车
    " " - 普通空白字符
*/

const string ALL_WHITE_SPACE = " \0\t\n\x0B\r";

static const char *g_pCodes =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static const unsigned char g_pMap[256] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
	 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};

int str_split_by(const std::string& src_str, std::vector<std::string>& sub_strings, const char delemiter /*= CHAR_SPACE*/)
{
    sub_strings.clear();

    if (src_str.size() == 0)  return 0;

    string::size_type  pos = 0;
    string::size_type  flag_pos = src_str.find_first_of(delemiter);

    while (flag_pos != string::npos) 
    {
        sub_strings.push_back(src_str.substr(pos, flag_pos-pos));
        pos = flag_pos + 1;

        flag_pos = src_str.find_first_of(delemiter, pos);
    }
    sub_strings.push_back(src_str.substr(pos));

    return (int)sub_strings.size();
}

const std::string join(const std::vector<std::string>& sub_strings, char delimiter)
{
    ostringstream  oss;
    if (sub_strings.size() == 0) return "";

    for (vector<string>::size_type index=0; index < sub_strings.size();)
    {
        oss << sub_strings.at(index);

        if (++index != sub_strings.size())
            oss << delimiter ;
    }

    return oss.str();
}

const std::string str_trim(const std::string& s)
{
    if (s.empty()) return s;

    std::string whitespaces = ALL_WHITE_SPACE;

    std::string::size_type first_non_space_pos = s.find_first_not_of(whitespaces); 
    std::string::size_type last_non_space_pos = s.find_last_not_of(whitespaces);

    if (first_non_space_pos == string::npos
        || last_non_space_pos == string::npos)
    {
        return "";
    }
    
    return s.substr(first_non_space_pos, 
        last_non_space_pos - first_non_space_pos + 1);
}

const std::string str_ltrim(const std::string& s)
{
    if (s.empty()) return s;

    std::string whitespaces = ALL_WHITE_SPACE;

    std::string::size_type first_non_space_pos = s.find_first_not_of(whitespaces);
    if (first_non_space_pos == string::npos)
    {
        return "";
    } 

    return s.substr(first_non_space_pos);
}

const std::string str_rtrim(const std::string& s)
{
    if (s.empty()) return s;

    std::string whitespaces = ALL_WHITE_SPACE;

    std::string::size_type last_non_space_pos = s.find_last_not_of(whitespaces);
    if (last_non_space_pos == string::npos)
    {
        return "";
    }

    return s.substr(0, last_non_space_pos + 1);
}


CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen)
{
	unsigned long i, len2, leven;
	unsigned char *p;

	if(pOut == NULL || *uOutLen == 0)
		return false;

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	len2 = ((uInLen + 2) / 3) << 2;
	if((*uOutLen) < (len2 + 1)) return false;

	p = pOut;
	leven = 3 * (uInLen / 3);
	for(i = 0; i < leven; i += 3)
	{
		*p++ = g_pCodes[pIn[0] >> 2];
		*p++ = g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		*p++ = g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		*p++ = g_pCodes[pIn[2] & 0x3f];
		pIn += 3;
	}

	if (i < uInLen)
	{
		unsigned char a = pIn[0];
		unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;
		unsigned char c = 0;

		*p++ = g_pCodes[a >> 2];
		*p++ = g_pCodes[((a & 3) << 4) + (b >> 4)];
		*p++ = ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		*p++ = '=';
	}

	*p = 0; // Append NULL byte
	*uOutLen = p - pOut;
	return true;
}

bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, std::string& strOut)
{
	unsigned long i, leven;
	strOut = "";

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	//len2 = ((uInLen + 2) / 3) << 2;
	//if((*uOutLen) < (len2 + 1)) return false;

	//p = pOut;
	leven = 3 * (uInLen / 3);
	for(i = 0; i < leven; i += 3)
	{
		strOut += g_pCodes[pIn[0] >> 2];
		strOut += g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		strOut += g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		strOut += g_pCodes[pIn[2] & 0x3f];
		pIn += 3;
	}

	if (i < uInLen)
	{
		unsigned char a = pIn[0];
		unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;
		unsigned char c = 0;

		strOut += g_pCodes[a >> 2];
		strOut += g_pCodes[((a & 3) << 4) + (b >> 4)];
		strOut += ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		strOut += '=';
	}

	//*p = 0; // Append NULL byte
	//*uOutLen = p - pOut;
	return true;
}

bool CBase64::Decode(const std::string& strIn, unsigned char *pOut, unsigned long *uOutLen)
{
	unsigned long t, x, y, z;
	unsigned char c;
	unsigned long g = 3;

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	for(x = y = z = t = 0; x < strIn.length(); x++)
	{
		c = g_pMap[(size_t)strIn[x]];
		if(c == 255) continue;
		if(c == 254) { c = 0; g--; }

		t = (t << 6) | c;

		if(++y == 4)
		{
			if((z + g) > *uOutLen) { return false; } // Buffer overflow
			pOut[z++] = (unsigned char)((t>>16)&255);
			if(g > 1) pOut[z++] = (unsigned char)((t>>8)&255);
			if(g > 2) pOut[z++] = (unsigned char)(t&255);
			y = t = 0;
		}
	}

	*uOutLen = z;
	return true;
}

std::vector<std::string> str_split(const std::string& src, std::string delimit, std::string null_subst )
{
	using namespace std;
	typedef basic_string<char>::size_type s_t;
	static const s_t npos = -1;
	vector<string> v;

	if (src.empty() || delimit.empty())
	{
		//throw "split:empty string\0";
		return v;
	}

	s_t deli_len = delimit.size();
	s_t index = npos, last_search_position = 0;
	while ((index = src.find(delimit, last_search_position)) != npos)
	{
		if (index == last_search_position)
		{
			v.push_back(null_subst);
		}
		else
		{
			v.push_back(src.substr(last_search_position, index
				- last_search_position));
		}
		last_search_position = index + deli_len;
	}
	string last_one = src.substr(last_search_position);
	v.push_back(last_one.empty() ? null_subst : last_one);
	return v;
}


int random(int min_num, int max_num)
{
	//assert(max_num >= min_num);
	//return (int) ((max_num - min_num + 1) * (rand() / (RAND_MAX + 1.0)));
	return rand() % abs(max_num - min_num + 1) + min_num;
}


time_t convert_string_to_time_t(const std::string & time_string)
{
	struct tm tm1;
	memset(&tm1, 0, sizeof(tm1));
	time_t time1;
	int i = sscanf(time_string.c_str(), "%4d%2d%2d %d:%d:%d",
		&(tm1.tm_year),
		&(tm1.tm_mon),
		&(tm1.tm_mday),
		&(tm1.tm_hour),
		&(tm1.tm_min),
		&(tm1.tm_sec));
    if (i!=6)
        return 0;
	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;

	time1 = mktime(&tm1);

	return time1;
}

std::string base64_str_to_normal(const std::string& base64_str)
{
	int len = base64_str.length();
	std::vector<unsigned char> data_buf_v;
	data_buf_v.resize(len);
	unsigned char* data_buf = data_buf_v.data();
	unsigned long out_len = len;
	CBase64::Decode(base64_str, data_buf, &out_len);
	std::string str_data((const char*)data_buf, out_len);
	return str_data;
}

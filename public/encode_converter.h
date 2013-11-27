/*******************************************************************************
 *
 *      @file: encode_converter.h
 *
 *      @brief: Convert encoding of given string from one encoding to another
 *              �ַ�������ת��
 *              details: $man 3 iconv
 *
 *      @author: gxl. 2012/9/27
 *
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#include <iconv.h>
#include <string>

enum { MAX_CONVER_BUFFER = 10240, };

// ����ת��������
class EncodeConverter 
{
public:
	EncodeConverter(const char *from_charset,const char *to_charset);
	~EncodeConverter();

	// ת�����
	int convert(char** inbuf, size_t* inlen, char** outbuf, size_t* outlen);

	int convert(char *inbuf,int inlen,char *outbuf,int outlen);
	int convert(const std::string src, std::string& dest);

private:
	iconv_t cd;
};

// Demo Usage:
//EncodeConverter  ec = EncodeConverter("gb2312", "UTF-8");
//
//char* str_1 = "������";
//char* str_2 = "�Ķ���";
//char buff_1[64] = {0};
//char buff_2[64] = {0};
//size_t len_str_1 = strlen(str_1);
//size_t len_str_2 = strlen(str_2);
//size_t len_buff = 64;
//
//int ret = 0;
//char* p_buff = buff_1;
//if ((ret = ec.convert(&str_1, &len_str_1, &p_buff, &len_buff)) == -1)
//{
//	LOG_ERR("encode convert failed: %s", strerror(errno));
//}
//
//len_buff = 64;
//p_buff = buff_2;
//if ((ret = ec.convert(&str_2, &len_str_2, &p_buff, &len_buff)) == -1)
//{
//	LOG_ERR("encode convert failed: %s", strerror(errno));
//}
//
//// in message channel
//// xxx ������ *** �Ķ���
//char buff[1024] = {0};
//sprintf(buff, CLIENT_MESSAGE_FORMAT"%s"CLIENT_MESSAGE_FORMAT"%s", 
//		player->get_role_id(), player->get_own_buddy()->get_buddy_name().c_str(), 
//		buff_1, t->get_leader()->get_role_id(), t->base_info.name().c_str(), buff_2);


/*******************************************************************************
 *
 *      @file: encode_converter.cpp
 *
 *      @brief: Convert encoding of given string from one encoding to another
 *              ×Ö·û´®±àÂë×ª»»
 *              details: $man 3 iconv
 *
 *      @author: gxl. 2012/9/27
 *
 *      COPYRIGHT (C) 2012, KingNet INC., ALL RIGHT RESERVED.
 ******************************************************************************/

#include "encode_converter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

EncodeConverter::EncodeConverter(const char *from_charset,const char *to_charset) 
{
	cd = iconv_open(to_charset,from_charset);
}

// Îö¹¹
EncodeConverter::~EncodeConverter() 
{
	iconv_close(cd);
}

// ×ª»»Êä³ö
int EncodeConverter::convert(char** inbuf, size_t* inlen, char** outbuf, size_t* outlen) 
{
	return iconv(cd, inbuf, inlen, outbuf, outlen);
}

int EncodeConverter::convert(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char **pin = &inbuf;
	char **pout = &outbuf;

	memset(outbuf,0,outlen);
	return iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen);
}

int EncodeConverter::convert(const std::string src, std::string& dest)
{
	if (src.size() >= MAX_CONVER_BUFFER)
	{
		return -1;
	}
	
	int  inlen=(int)src.size(), out_len=MAX_CONVER_BUFFER;

	char in_buff[MAX_CONVER_BUFFER] = {0};
	sprintf(in_buff, "%s", src.c_str());

	char out_buff[MAX_CONVER_BUFFER] = {0};

	char*  p_buff = in_buff;
	
	return convert(p_buff, inlen, out_buff, out_len);

}

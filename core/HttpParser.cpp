// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpParser.h"

namespace sealedserver
{

bool HttpParser::parse(Buffer &receivedBuff)
{
	string str = (string)receivedBuff;

	origin_ = str;
	
	int index; bool flag;
	flag = parseFirstLine(str, index);

	if(flag == false){
		return false;
	} 

	flag = check();
	if(flag == false){
		return false;
	} 

	flag = parseURL();
	if(flag == false){
		return false;
	} 

	str  = str.substr(index);
	flag = parseHeader(str);

	if(flag == false) 
		return false;

	return true;
}

bool HttpParser::parseFirstLine(const string &str, int &index)
{
	int i = 0, len = str.size(), j;
	
	/// skip space
	for(;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;
	
	/// Find the first space
  	for(j = i + 1;j < len && str.at(j) != ' ';j++);
  	if(j >= len) return false;

  	method_ = str.substr(i, j - i);

  	/// skip space
  	for(i = j;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;
	
	/// Find the first space
  	for(j = i + 1;j < len && str.at(j) != ' ';j++);
  	if(j >= len) return false;

  	url_ = str.substr(i, j - i);

  	/// skip space
  	for(i = j;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;

	/// Find the `\r` space
 	for(j = i + 1;j < len && str.at(j) != '\r';j++);
 	if(j >= len - 1 || str.at(j + 1) != '\n') return false;
	
  	version_ = str.substr(i, j - i);	

  	index = j + 2;

  	return true;
}

bool HttpParser::parseHeader(string str)
{
	bool flag = true;
	while(str.size() != 0)
	{
		int index = str.find("\r\n");
		if(index == -1) {
			return false;
		} 

		if(index == 0) break;
		
		flag = parseLine(str.substr(0, index));
		
		if(flag == false){
			return false;
		}

		str = str.substr(index + 2);
	}

	return true;
}

bool HttpParser::parseLine(string str)
{	
	int index = str.find(": ");
	
	if(index == -1 || index == 0 || index >= str.size() - 2)
		return false;

	header_[str.substr(0,index)] = str.substr(index + 2);
}

/// only support get
int HttpParser::is_valid_http_method(const char *s) {
  return !strcmp(s, "GET");
}

bool HttpParser::check()
{
	if(is_valid_http_method(method_.c_str()) == false) 
		return false;

	if (memcmp(version_.c_str(), "HTTP/", 5) != 0)
		return false;

	return true;
}

bool HttpParser::parseURL()
{
	int index = url_.find("?");
	bool flag = true;

	if(index == -1)
		querystring_ = url_;
	else
	{
		querystring_ = url_.substr(0, index);
		flag = decode(url_.substr(index + 1));
	}

	return flag;
}

#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

bool HttpParser::decode(string para)
{
	int i, a, b; string decodestr;
	int len = para.size();
		
	for(i = 0; i < len;i++)
	{
		if(para.at(i) == '%' && i < para.size() -2 &&
			isxdigit(para.at(i+1)) && isxdigit(para.at(i+2)))
		{
			a = tolower(para.at(i + 1));
			b = tolower(para.at(i + 2));
			decodestr.push_back((char) ((HEXTOI(a) << 4) | HEXTOI(b)));
			i += 2;
		}
		else if(para.at(i) == '+'){
			decodestr.push_back(' ');
		}
		else{
			decodestr.push_back(para.at(i));
		}
	}

	bool flag = true;

	while(flag == true)
	{
		int index = decodestr.find("&");
		if(index == -1)
		{
			flag = parsekv(decodestr);
			break;
		}
		flag = parsekv(decodestr.substr(0, index));
		decodestr = decodestr.substr(index + 1);
	}

	return flag;
}

bool HttpParser::parsekv(string str)
{
	int index = str.find("=");
	if(index == -1 || index == 0)
		return false;
	params_[str.substr(0, index)] = str.substr(index + 1);

	return true;
}

};
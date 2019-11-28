#ifndef _IDCARD_HTTP_UTIL_H_
#define _IDCARD_HTTP_UTIL_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <winhttp.h>
#include <string>

class HttpsUtil
{
public:
	HttpsUtil(LPWSTR url);
	virtual ~HttpsUtil();
public:
	int Open(int timeout = 60000);
	int Post(const std::string& input_data, std::wstring& output_data);
	void Close();
private:
	std::wstring	_url;
	URL_COMPONENTS	_url_components;
	HINTERNET		_session;
	HINTERNET		_connect;
};

#endif // _IDCARD_HTTP_UTIL_H_
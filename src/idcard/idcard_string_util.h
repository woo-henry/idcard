#ifndef _IDCARD_STRING_UTIL_H_
#define _IDCARD_STRING_UTIL_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <string>

std::wstring StrA2Unicode(const std::string& ansi_string);
std::string StrUnicode2UTF8(const std::wstring& unicode_string);
std::string StrA2UTF8(const std::string& ansi_string);

std::string StrUnicode2A(const std::wstring& unicode_string);
std::wstring StrUTF8Unicode(const std::string& utf8_string);
std::string StrUTF82A(const std::string& utf8_string);

#endif // _IDCARD_STRING_UTIL_H_
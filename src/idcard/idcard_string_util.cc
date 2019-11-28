#include <windows.h>
#include "lib_base/lib_base.h"
#include "idcard_string_util.h"
//////////////////////////////////////////////////////////////////////////
std::wstring StrA2Unicode(const std::string& ansi_string)
{
	int unicode_string_legnth = MultiByteToWideChar(CP_ACP, 0, ansi_string.c_str(), -1, nullptr, 0);
	wchar_t* unicode_buffer = (wchar_t*)BaseMemAlloc(sizeof(wchar_t) * unicode_string_legnth);
	MultiByteToWideChar(CP_ACP, 0, ansi_string.c_str(), -1, unicode_buffer, unicode_string_legnth);
	std::wstring unicode_string = unicode_buffer;
	BaseMemFree(unicode_buffer);
	return unicode_string;
}

std::string StrUnicode2UTF8(const std::wstring& unicode_string)
{
	int utf8_string_length = WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* utf8_buffer = (char*)BaseMemAlloc(sizeof(char) * utf8_string_length);
	WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, utf8_buffer, utf8_string_length, nullptr, nullptr);
	std::string utf8_string = utf8_buffer;
	BaseMemFree(utf8_buffer);
	return utf8_string;
}

std::string StrA2UTF8(const std::string& ansi_string)
{
	return StrUnicode2UTF8(StrA2Unicode(ansi_string));
}

std::string StrUnicode2A(const std::wstring& unicode_string)
{
	int ansi_string_length = WideCharToMultiByte(CP_ACP, 0, unicode_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* ansi_buffer = (char*)BaseMemAlloc(sizeof(char) * ansi_string_length);
	WideCharToMultiByte(CP_ACP, 0, unicode_string.c_str(), -1, ansi_buffer, ansi_string_length, nullptr, nullptr);
	std::string ansi_string = ansi_buffer;
	BaseMemFree(ansi_buffer);
	return ansi_string;
}

std::wstring StrUTF8Unicode(const std::string& utf8_string)
{
	int unicode_string_legnth = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, nullptr, 0);
	wchar_t* unicode_buffer = (wchar_t*)BaseMemAlloc(sizeof(wchar_t) * unicode_string_legnth);
	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, unicode_buffer, unicode_string_legnth);
	std::wstring unicode_string = unicode_buffer;
	BaseMemFree(unicode_buffer);
	return unicode_string;
}

std::string StrUTF82A(const std::string& utf8_string)
{
	return StrUnicode2A(StrUTF8Unicode(utf8_string));
}
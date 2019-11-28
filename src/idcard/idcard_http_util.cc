#include <windows.h>
#include "lib_base/lib_base.h"
#include "idcard_http_util.h"
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "winhttp.lib")

HttpsUtil::HttpsUtil(const char* url)
	: _url(url)
	, _session(nullptr)
	, _connect(nullptr)
{
	ZeroMemory(&_url_components, sizeof(URL_COMPONENTS));
	_url_components.dwStructSize = sizeof(URL_COMPONENTS);
	_url_components.dwSchemeLength = (DWORD)-1;
	_url_components.dwHostNameLength = (DWORD)-1;
	_url_components.dwUrlPathLength = (DWORD)-1;
	_url_components.dwExtraInfoLength = (DWORD)-1;
}

HttpsUtil::~HttpsUtil()
{
	Close();
}

int HttpsUtil::Open(int timeout)
{
	int result = EXIT_SUCCESS;
	wchar_t* url = nullptr;

	do
	{
		if(_session != nullptr || _connect != nullptr)
			break;

		_session = WinHttpOpen(L"A WinHTTP download Program/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (_session == nullptr)
		{
			result = GetLastError();
			break;
		}
		
		int url_size = 0;
		if (!BaseStrA2W(url, &url_size, _url.c_str()))
		{
			result = ERROR_DATA_LOST_REPAIR;
			break;
		}

		url = (wchar_t*)BaseMemAlloc(url_size);
		if (url == nullptr)
		{
			result = ERROR_OUTOFMEMORY;
			break;
		}

		if (!BaseStrA2W(url, &url_size, _url.c_str()))
		{
			result = ERROR_DATA_LOST_REPAIR;
			break;
		}

		int url_legnth = lstrlenW(url);
		if (!WinHttpCrackUrl(url, (DWORD)url_legnth, 0, &_url_components))
		{
			result = GetLastError();
			break;
		}

		wchar_t host_name[32] = { 0 };
		memcpy(host_name, _url_components.lpszHostName, _url_components.dwHostNameLength * sizeof(wchar_t));
		_connect = WinHttpConnect(_session, host_name, _url_components.nPort, 0);
		if (_connect == nullptr)
		{
			result = GetLastError();
			break;
		}

		if (!WinHttpSetTimeouts(_session, timeout, timeout, timeout, timeout))
		{
			result = GetLastError();
			break;
		}
		
	} while (false);

	if (url)
	{
		BaseMemFree(url);
		url = nullptr;
	}

	return result;
}

int HttpsUtil::Post(const std::string& input_data, std::string& output_data)
{
	int result = EXIT_SUCCESS;
	HINTERNET request = nullptr;

	do 
	{
		wchar_t url_path[MAX_PATH] = { 0 };
		int url_size = 0;
		if (!BaseStrA2W(url_path, &url_size, _url.c_str()))
		{
			result = ERROR_DATA_LOST_REPAIR;
			break;
		}
		//memcpy(url_path, _url_components.lpszUrlPath, _url_components.dwUrlPathLength * sizeof(wchar_t));
		request = WinHttpOpenRequest(_connect, L"POST", url_path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if (request == nullptr)
		{
			result = GetLastError();
			break;
		}

		std::wstring header;
		header.append(L"Connection: close");
		if (!WinHttpAddRequestHeaders(request, header.c_str(), (DWORD)header.length(), WINHTTP_ADDREQ_FLAG_ADD))
		{
			result = GetLastError();
			break;
		}

		header.clear();
		header.append(L"Content-type: application/json");
		if (!WinHttpAddRequestHeaders(request, header.c_str(), (DWORD)header.length(), WINHTTP_ADDREQ_FLAG_ADD))
		{
			result = GetLastError();
			break;
		}

		DWORD input_length = (DWORD)input_data.length();
		if (!WinHttpSendRequest(request, header.c_str(), header.length(), (LPVOID)input_data.c_str(), input_length, input_length, 0))
		{
			result = ERROR_WINHTTP_CANNOT_CONNECT;
			result = GetLastError();
			break;
		}

		if (!WinHttpReceiveResponse(request, NULL))
		{
			result = GetLastError();
			break;
		}

		result = ReadData(request, output_data);

	} while (false);

	if (request)
	{
		WinHttpCloseHandle(request);
		request = nullptr;
	}

	return result;
}

void HttpsUtil::Close()
{
	if (_connect)
	{
		WinHttpCloseHandle(_connect);
		_connect = nullptr;
	}

	if (_session)
	{
		WinHttpCloseHandle(_session);
		_session = nullptr;
	}
}

int HttpsUtil::ReadData(HINTERNET request, std::string& output_data)
{
	int result = ERROR_SUCCESS;
	char* output_buffer = nullptr;
	DWORD available_size = 0;

	do
	{		
		if (!WinHttpQueryDataAvailable(request, &available_size)) 
		{
			result = GetLastError();
			break;
		}
			
		output_buffer = (char*)BaseMemAlloc(available_size + 1);
		if (!output_buffer)
		{
			result = ERROR_OUTOFMEMORY;
			break;
		}
		
		DWORD number_bytes_read = 0;
		if (!WinHttpReadData(request, (LPVOID)output_buffer, available_size, &number_bytes_read))
		{
			result = GetLastError();
			break;
		}
			
		output_data.append(output_buffer);

		BaseMemFree(output_buffer);
		output_buffer = nullptr;

	} while (available_size > 0);

	return result;
}
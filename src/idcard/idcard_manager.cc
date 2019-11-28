#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#include <tinyxml2.h>
#include <json/json.h>
#include <curl/curl.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include "lib_base/lib_base.h"
#include "idcard_string_util.h"
#include "idcard_http_util.h"
#include "idcard_error.h"
#include "idcard_define.h"
#include "idcard_config.h"
#include "idcard_module.h"
#include "idcard_manager.h"
//////////////////////////////////////////////////////////////////////////
static boost::shared_ptr<IdCardManager> _instance = nullptr;
static boost::mutex _instance_mutex;
boost::shared_ptr<IdCardManager>& IdCardManager::GetInstance()
{
	if (_instance == nullptr)
	{
		boost::lock_guard<boost::mutex> lock(_instance_mutex);
		if (_instance == nullptr)
		{
			_instance.reset(new IdCardManager);
		}
	}

	return _instance;
}
//////////////////////////////////////////////////////////////////////////
IdCardManager::IdCardManager()
{

}

IdCardManager::~IdCardManager()
{
	Dispose();
}

int IdCardManager::Init(boost::shared_ptr<IdCardConfig>& config)
{
	int result = IDCARD_ERROR_SUCCESS;

	int idcard_count = config->GetIdCardCount();
	for (int i = 0; i < idcard_count; i++)
	{
		IdCardConfigItem* item = (IdCardConfigItem*)config->GetIdCard(i);
		if (item == nullptr)
		{
			BaseLogWarn("IdCard manager manufactor[%s] module[%s] not exist",
				item->GetManufactorName(), item->GetModuleName());
			continue;
		}

		IdCardModule* module = new IdCardModule(item);
		result = module->Init();
		if (result != IDCARD_ERROR_SUCCESS)
		{
			BaseLogWarn("IdCard manager init manufactor[%s] module[%s] failed.",
				item->GetManufactorName(), item->GetModuleName());
			continue;
		}

		_idcard_modules.push_back(module);
	}

	return result;
}

void IdCardManager::Dispose()
{
	if (!_idcard_modules.empty())
	{
		for (auto it = _idcard_modules.begin(); it != _idcard_modules.end(); it++)
		{
			IdCardModule* module = *it;
			if (module)
			{
				module->Dispose();
				module = nullptr;
			}
		}

		_idcard_modules.erase(_idcard_modules.begin(), _idcard_modules.end());
	}
}

int IdCardManager::ReadCard(const char* output_target, const char* encoding, bool read_photo)
{
	int result = IDCARD_ERROR_SUCCESS;

	for (auto it = _idcard_modules.begin(); it != _idcard_modules.end(); it++)
	{
		IdCardModule* module = *it;
		if (module)
		{
			int port = 0;
			result = module->Open(&port);
			if (result != IDCARD_ERROR_SUCCESS)
				continue;

			result = module->ReadCardInfo(read_photo);
			if (result == IDCARD_ERROR_SUCCESS)
			{
				const char* people_name = module->GetPeopleName();
				const char* people_sex = module->GetPeopleSex();
				const char* people_nation = module->GetPeopleNation();
				const char* people_birthday = module->GetPeopleBirthday();
				const char* people_address = module->GetPeopleAddress();
				const char* people_id_code = module->GetPeopleIdCode();
				const char* sign_department = module->GetSignDepartment();
				const char* valid_start_date = module->GetValidStartDate();
				const char* valid_end_date = module->GetValidEndDate();
				const char* bitmap_data = module->GetBitmapData();
				int bitmap_size = module->GetBitmapSize();

				if(boost::istarts_with(output_target, "http://") || boost::istarts_with(output_target, "https://"))
				{
					result = OutputNetwork(output_target, encoding, people_name, people_sex, people_nation,
						people_birthday, people_address, people_id_code, sign_department,
						valid_start_date, valid_end_date, bitmap_data, bitmap_size);
				}
				else
				{
					result = OutputFile(output_target, people_name, people_sex, people_nation,
						people_birthday, people_address, people_id_code, sign_department,
						valid_start_date, valid_end_date, bitmap_data, bitmap_size);
				}
				
				module->Close();
				break;
			}

			module->Close();
		}
	}

	return result;
}

int IdCardManager::OutputFile(const char* output_file,
	const char* people_name,
	const char* people_sex,
	const char* people_nation,
	const char* people_birthday,
	const char* people_address,
	const char* people_id_code,
	const char* sign_department,
	const char* valid_start_date,
	const char* valid_end_date,
	const char* bitmap_data,
	int bitmap_size)
{
	int result = IDCARD_ERROR_SUCCESS;
	HANDLE json_file_handle = INVALID_HANDLE_VALUE;
	HANDLE bitmap_file_handle = INVALID_HANDLE_VALUE;
	char bitmap_file[MAX_PATH] = { 0 };

	do 
	{
		Json::Value json_data;
		json_data["people_name"] = people_name;
		json_data["people_sex"] = people_sex;
		json_data["people_nation"] = people_nation;
		json_data["people_birthday"] = people_birthday;
		json_data["people_address"] = people_address;
		json_data["people_id_code"] = people_id_code;
		json_data["sign_department"] = sign_department;
		json_data["valid_start_date"] = valid_start_date;
		json_data["valid_end_date"] = valid_end_date;

		Json::StreamWriterBuilder builder;
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		std::ostringstream os;
		writer->write(json_data, &os);
		std::string json = os.str();

		json_file_handle = ::CreateFileA(
			output_file,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (json_file_handle == INVALID_HANDLE_VALUE)
		{
			result = IDCARD_ERROR_FILE_CREATE_FAILURE;
			break;
		}

		unsigned long number_of_bytes_write = 0;
		if (!::WriteFile(json_file_handle, (char*)json.c_str(), (unsigned long)json.length(), &number_of_bytes_write, nullptr))
		{
			result = IDCARD_ERROR_FILE_WRITE_FAILURE;
			break;
		}

		lstrcpyA(bitmap_file, output_file);
		lstrcatA(bitmap_file, ".bmp");

		bitmap_file_handle = ::CreateFileA(
			bitmap_file,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (bitmap_file_handle == INVALID_HANDLE_VALUE)
		{
			result = IDCARD_ERROR_FILE_CREATE_FAILURE;
			break;
		}

		if (!::WriteFile(bitmap_file_handle, (char*)bitmap_data, (unsigned long)bitmap_size, &number_of_bytes_write, nullptr))
		{
			result = IDCARD_ERROR_FILE_WRITE_FAILURE;
			break;
		}

	} while (false);

	if (json_file_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(json_file_handle);
		json_file_handle = INVALID_HANDLE_VALUE;
	}

	return result;
}

int IdCardManager::OutputNetwork(const char* output_url,
	const char* encoding,
	const char* people_name,
	const char* people_sex,
	const char* people_nation,
	const char* people_birthday,
	const char* people_address,
	const char* people_id_code,
	const char* sign_department,
	const char* valid_start_date,
	const char* valid_end_date,
	const char* bitmap_data,
	int bitmap_size)
{
	int result = IDCARD_ERROR_SUCCESS;
	char* bitmap_data_base64 = nullptr;
	char* curl_path = nullptr;
	char* app_path = nullptr;
	char* request_filename = nullptr;
	HANDLE request_file_handle = nullptr;
	char* response_filename = nullptr;
	char* response_buffer = nullptr;
	char* response_buffer_ansi = nullptr;
	wchar_t* response_buffer_utf8 = nullptr;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	do
	{
		curl_path = (char*)BaseMemAlloc(MAX_PATH);
		if (curl_path == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc curl path buffer failure, result : %d", result);
			break;
		}

		if (!BaseFsGetAppPathA(curl_path, "curl.exe"))
		{
			result = IDCARD_ERROR_GET_APP_CURL_PATH;
			BaseLogError("IdCardCollector output network get curl path failure, result : %d", result);
			break;
		}

		app_path = (char*)BaseMemAlloc(MAX_PATH);
		if (app_path == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc app path buffer failure, result : %d", result);
			break;
		}

		if (!BaseFsGetAppPathA(app_path, nullptr))
		{
			result = IDCARD_ERROR_GET_APP_PATH;
			BaseLogError("IdCardCollector output network get app path failure, result : %d", result);
			break;
		}

		request_filename = (char*)BaseMemAlloc(MAX_PATH);
		if (request_filename == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc request filename buffer failure, result : %d", result);
			break;
		}

		unsigned int ret = GetTempFileNameA(app_path, "req000", 0, request_filename);
		if (ret == 0)
		{
			result = IDCARD_ERROR_GET_REQUEST_FILENAME_FAILURE;
			BaseLogError("IdCardCollector output network get request filename failure, result : %d", result);
			break;
		}

		response_filename = (char*)BaseMemAlloc(MAX_PATH);
		if (response_filename == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc response filename buffer failure, result : %d", result);
			break;
		}

		ret = GetTempFileNameA(app_path, "res000", 0, response_filename);
		if (ret == 0)
		{
			result = IDCARD_ERROR_GET_RESPONSE_FILENAME_FAILURE;
			BaseLogError("IdCardCollector output network get response filename failure, result : %d", result);
			break;
		}

		bitmap_data_base64 = (char*)BaseMemAlloc(bitmap_size * 2);
		if (bitmap_data_base64 == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc bitmap base64 buffer failure, result : %d", result);
			break;
		}

		size_t bitmap_data_base64_length = 0;
		BaseStrEncodeBase64(bitmap_data_base64, &bitmap_data_base64_length, bitmap_data, bitmap_size);
		std::string bitmap_data_base64_string;
		bitmap_data_base64_string.assign(bitmap_data_base64, bitmap_data_base64_length);

		Json::Value json_data;
		json_data["name"] = StrA2UTF8(people_name);
		json_data["sex"] = StrA2UTF8(people_sex);
		json_data["national"] = StrA2UTF8(people_nation);
		json_data["idcard_add"] = StrA2UTF8(people_address);
		json_data["issuance"] = StrA2UTF8(sign_department);
		json_data["birthday"] = people_birthday;
		json_data["idcard_no"] = people_id_code;
		json_data["start_date"] = valid_start_date;
		json_data["end_date"] = valid_end_date;
		json_data["idcard_img"] = bitmap_data_base64_string;
		std::string request_string = json_data.toStyledString();

		request_file_handle = ::CreateFileA(
			request_filename,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (request_file_handle == INVALID_HANDLE_VALUE)
		{
			result = IDCARD_ERROR_WRITE_REQUEST_FILE_FAILURE;
			BaseLogError("IdCardCollector output network create request file failure, result : %d", result);
			break;
		}

		unsigned int number_of_bytes_write = 0;
		if (!::WriteFile(request_file_handle, (char*)request_string.c_str(), (unsigned long)request_string.length(), (PDWORD)&number_of_bytes_write, nullptr))
		{
			result = IDCARD_ERROR_WRITE_REQUEST_FILE_FAILURE;
			BaseLogError("IdCardCollector output network write request file failure, result : %d", result);
			break;
		}

		std::string curl_command;
		curl_command.append("\"");
		curl_command.append(curl_path);
		curl_command.append("\"");
		curl_command.append(" -X POST -H \"content-type: application/json\" ");
		curl_command.append(" -d ");
		curl_command.append("@").append(request_filename);
		curl_command.append(" \"");
		curl_command.append(output_url);
		curl_command.append("\" ");
		curl_command.append(" -o ");
		curl_command.append("\"");
		curl_command.append(response_filename);
		curl_command.append("\"");

		STARTUPINFOA si;
		ZeroMemory(&si, sizeof(STARTUPINFOA));
		si.cb = sizeof(STARTUPINFOA);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwX = 0;
		si.dwY = 0;
		si.dwXSize = 0;
		si.dwYSize = 0;
		si.dwXCountChars = 500;
		si.dwYCountChars = 500;
		si.dwFlags |= STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		si.cbReserved2 = 0;
		si.lpReserved2 = NULL;
		BaseLogInfo("IdCardCollector create process by command : %s", curl_command.c_str());
		if (!CreateProcessA(nullptr, (LPSTR)curl_command.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
		{
			result = IDCARD_ERROR_CREATE_PIPE_FAILURE;
			BaseLogError("IdCardCollector output network create process failure, result : %d", result);
			break;
		}

		unsigned int wait_result = WaitForSingleObject(pi.hThread, INFINITE);
		if (wait_result != WAIT_OBJECT_0)
		{
			result = IDCARD_ERROR_WAIT_THREAD_FAILURE;
			BaseLogError("IdCardCollector output network cannot receive thread signal, result : %d", result);
			break;
		}

		wait_result = WaitForSingleObject(pi.hProcess, INFINITE);
		if (wait_result != WAIT_OBJECT_0)
		{
			result = IDCARD_ERROR_WAIT_PROCESS_FAILURE;
			BaseLogError("IdCardCollector output network cannot receive process signal, result : %d", result);
			break;
		}

		DWORD curl_result = IDCARD_ERROR_SUCCESS;
		GetExitCodeProcess(pi.hProcess, &curl_result);
		BaseLogInfo("IdCardCollector get process exit code : %d", curl_result);
		if (curl_result != IDCARD_ERROR_SUCCESS)
		{
			result = IDCARD_ERROR_EXECUTE_CURL_FAILURE;
			BaseLogError("IdCardCollector output network execute curl failure, curl result : %d, result : %d", curl_result, result);
			break;
		}

		size_t response_buffer_size = 0;
		if (!BaseFsReadFileA(response_filename, response_buffer, &response_buffer_size))
		{
			result = IDCARD_ERROR_READ_PIPE_FAILURE;
			BaseLogError("IdCardCollector output network read pipe failure, result : %d", result);
			break;
		}

		response_buffer = (char*)BaseMemAlloc(response_buffer_size);
		if (response_buffer == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc response buffer failure, result : %d", result);
			break;
		}

		if (!BaseFsReadFileA(response_filename, response_buffer, &response_buffer_size))
		{
			result = IDCARD_ERROR_READ_PIPE_FAILURE;
			BaseLogError("IdCardCollector output network read pipe failure, result : %d", result);
			break;
		}

		int response_buffer_utf8_length = 0;
		if (!BaseStrA2W(response_buffer_utf8, &response_buffer_utf8_length, response_buffer, CP_UTF8))
		{
			result = IDCARD_ERROR_STRING_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network convert response to utf8 failure, result : %d", result);
			break;
		}

		response_buffer_utf8 = (wchar_t*)BaseMemAlloc(response_buffer_utf8_length);
		if (response_buffer_utf8 == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc response utf8 buffer failure, result : %d", result);
			break;
		}

		if (!BaseStrA2W(response_buffer_utf8, &response_buffer_utf8_length, response_buffer, CP_UTF8))
		{
			result = IDCARD_ERROR_STRING_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network convert response to utf8 failure, result : %d", result);
			break;
		}

		int response_buffer_ansi_length = 0;
		if (!BaseStrW2A(response_buffer_ansi, &response_buffer_ansi_length, response_buffer_utf8))
		{
			result = IDCARD_ERROR_STRING_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network convert response to ansi failure, result : %d", result);
			break;
		}

		response_buffer_ansi = (char*)BaseMemAlloc(response_buffer_ansi_length);
		if (response_buffer_ansi == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			BaseLogError("IdCardCollector output network alloc response ansi buffer failure, result : %d", result);
			break;
		}

		if (!BaseStrW2A(response_buffer_ansi, &response_buffer_ansi_length, response_buffer_utf8))
		{
			result = IDCARD_ERROR_STRING_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network convert response to ansi failure, result : %d", result);
			break;
		}

		BaseLogInfo("IdCardCollector output network response : %s", response_buffer_ansi);

		JSONCPP_STRING errs;
		Json::Value root;
		Json::CharReaderBuilder readerBuilder;
		std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
		bool res = jsonReader->parse(response_buffer_ansi, response_buffer_ansi + response_buffer_ansi_length, &root, &errs);
		if (!res)
		{
			result = IDCARD_ERROR_JSON_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network parse response to json failure, result : %d", result);
			break;
		}

		Json::Value json_status = root["status"];
		if (json_status.isNull() || json_status.empty())
		{
			result = IDCARD_ERROR_JSON_CONVERT_FAILURE;
			BaseLogError("IdCardCollector output network parse response status to json failure, result : %d", result);
			break;
		}

		int status = StrToIntA(json_status.asCString());
		if (status != 200)
		{
			result = status;
			BaseLogError("IdCardCollector output network failure, result : %d", result);
			break;
		}

	} while (false);

	if (pi.hThread)
	{
		CloseHandle(pi.hThread);
		pi.hThread = nullptr;
	}

	if (pi.hProcess)
	{
		CloseHandle(pi.hProcess);
		pi.hProcess = nullptr;
	}

	if (response_buffer_ansi)
	{
		BaseMemFree(response_buffer_ansi);
		response_buffer_ansi = nullptr;
	}

	if (response_buffer_utf8)
	{
		BaseMemFree(response_buffer_utf8);
		response_buffer_utf8 = nullptr;
	}

	if (response_buffer)
	{
		BaseMemFree(response_buffer);
		response_buffer = nullptr;
	}

	if (bitmap_data_base64)
	{
		BaseMemFree(bitmap_data_base64);
		bitmap_data_base64 = nullptr;
	}

	if (response_filename)
	{
#ifndef _DEBUG
		if (PathFileExistsA(response_filename))
		{
			DeleteFileA(response_filename);
		}
#endif
		BaseMemFree(response_filename);
		response_filename = nullptr;
	}

	if (request_file_handle)
	{
		CloseHandle(request_file_handle);
		request_file_handle = INVALID_HANDLE_VALUE;
	}

	if (request_filename)
	{
#ifndef _DEBUG
		if (PathFileExistsA(request_filename))
		{
			DeleteFileA(request_filename);
		}
#endif
		BaseMemFree(request_filename);
		request_filename = nullptr;
	}

	if (app_path)
	{
		BaseMemFree(app_path);
		app_path = nullptr;
	}

	if (curl_path)
	{
		BaseMemFree(curl_path);
		app_path = nullptr;
	}

	return result;
}
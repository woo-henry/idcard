#include <shlwapi.h>
#include <boost/algorithm/string.hpp>
#include <lib_base/lib_base.h>
#include "idcard_error.h"
#include "idcard_module.h"
//////////////////////////////////////////////////////////////////////////
IdCardModule::IdCardModule(const IdCardConfigItem* config_item)
	: _module_handle(nullptr)
	, _config_item(config_item)
	, _bitmap_size(0)
{
	ZeroMemory(_bitmap_data, MAX_BITMAP_DATA_SIZE);
}

IdCardModule::~IdCardModule()
{
	Close();

	if (_module_handle)
	{
		FreeLibrary(_module_handle);
		_module_handle = nullptr;
	}
}

void IdCardModule::Dispose()
{
	delete this;
}

int IdCardModule::Init()
{
	int result = IDCARD_ERROR_SUCCESS;
	char* sdk_path = nullptr;
	char* module_file = nullptr;

	do
	{
		sdk_path = (char*)BaseMemAlloc(MAX_PATH);
		if (sdk_path == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			break;
		}
#ifdef _DEBUG
		if (!BaseFsGetAppPathA(sdk_path, "..\\modules"))
#else
		if (!BaseFsGetAppPathA(sdk_path, "modules"))
#endif // _DEBUG
		{
			result = IDCARD_ERROR_GET_APP_PATH;
			break;
		}

		PathAddBackslashA(sdk_path);
		lstrcatA(sdk_path, _config_item->GetModulePath());

		if (!BaseSysSetEnvironmentVariableA("PATH", sdk_path))
		{
			result = IDCARD_ERROR_PATH_SET_ENVIRONMENT;
			break;
		}

		module_file = (char*)BaseMemAlloc(MAX_PATH);
		if (module_file == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			break;
		}

		if (!BaseFsGetAppPathA(module_file, _config_item->GetModuleName()))
		{
			result = IDCARD_ERROR_GET_APP_PATH;
			break;
		}

		if (!boost::iends_with(module_file, ".dll"))
		{
			lstrcatA(module_file, ".dll");
		}

		_module_handle = LoadLibraryA(module_file);
		if (_module_handle == nullptr)
		{
			result = IDCARD_ERROR_MODULE_LOAD_FAILURE;
			break;
		}

		_pfnIdCard_Init = (IdCard_Init)GetProcAddress(_module_handle, "IdCard_Init");
		if (_pfnIdCard_Init == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}
		
		_pfnIdCard_Open = (IdCard_Open)GetProcAddress(_module_handle, "IdCard_Open");
		if (_pfnIdCard_Open == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_Close = (IdCard_Close)GetProcAddress(_module_handle, "IdCard_Close");
		if (_pfnIdCard_Close == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_ReadInfo = (IdCard_ReadInfo)GetProcAddress(_module_handle, "IdCard_ReadInfo");
		if (_pfnIdCard_ReadInfo == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_Close = (IdCard_Close)GetProcAddress(_module_handle, "IdCard_Close");
		if (_pfnIdCard_Close == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleName = (IdCard_GetPeopleName)GetProcAddress(_module_handle, "IdCard_GetPeopleName");
		if (_pfnIdCard_GetPeopleName == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleSex = (IdCard_GetPeopleSex)GetProcAddress(_module_handle, "IdCard_GetPeopleSex");
		if (_pfnIdCard_GetPeopleSex == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleNation = (IdCard_GetPeopleNation)GetProcAddress(_module_handle, "IdCard_GetPeopleNation");
		if (_pfnIdCard_GetPeopleNation == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleBirthday = (IdCard_GetPeopleBirthday)GetProcAddress(_module_handle, "IdCard_GetPeopleBirthday");
		if (_pfnIdCard_GetPeopleBirthday == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleAddress = (IdCard_GetPeopleAddress)GetProcAddress(_module_handle, "IdCard_GetPeopleAddress");
		if (_pfnIdCard_GetPeopleAddress == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetPeopleIdCode = (IdCard_GetPeopleIdCode)GetProcAddress(_module_handle, "IdCard_GetPeopleIdCode");
		if (_pfnIdCard_GetPeopleIdCode == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetSignDepartment = (IdCard_GetSignDepartment)GetProcAddress(_module_handle, "IdCard_GetSignDepartment");
		if (_pfnIdCard_GetSignDepartment == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetValidStartDate = (IdCard_GetValidStartDate)GetProcAddress(_module_handle, "IdCard_GetValidStartDate");
		if (_pfnIdCard_GetValidStartDate == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetValidEndDate = (IdCard_GetValidEndDate)GetProcAddress(_module_handle, "IdCard_GetValidEndDate");
		if (_pfnIdCard_GetValidEndDate == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		_pfnIdCard_GetBitmapData = (IdCard_GetBitmapData)GetProcAddress(_module_handle, "IdCard_GetBitmapData");
		if (_pfnIdCard_GetBitmapData == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		result = _pfnIdCard_Init(_config_item->GetModulePath());

	} while (false);

	if (module_file)
	{
		BaseMemFree(module_file);
		module_file = nullptr;
	}

	if (sdk_path)
	{
		BaseMemFree(sdk_path);
		sdk_path = nullptr;
	}

	return result;
}

int IdCardModule::Open(int* port)
{
	if (_pfnIdCard_Open == nullptr)
		return IDCARD_ERROR_NULL_POINTER;

	return _pfnIdCard_Open(port);
}

int IdCardModule::Close()
{
	int result = _pfnIdCard_Close();

	_people_name.clear();
	_people_sex.clear();
	_people_nation.clear();
	_people_birthday.clear();
	_people_address.clear();
	_people_id_code.clear();
	_sign_department.clear();
	_valid_start_date.clear();
	_valid_end_date.clear();
	ZeroMemory(_bitmap_data, MAX_BITMAP_DATA_SIZE);
	_bitmap_size = 0;

	return result;
}

int IdCardModule::ReadCardInfo(bool read_photo)
{
	int result = IDCARD_ERROR_SUCCESS;
	unsigned char content[MAX_PATH] = { 0 };
	unsigned char bitmap_data[MAX_BITMAP_DATA_SIZE] = { 0 };
	int length = 0;

	do 
	{
		int ret = _pfnIdCard_ReadInfo();
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_INFO_FAILURE;
			break;
		}

		ZeroMemory(content, MAX_PATH);
		_people_name.clear();
		ret = _pfnIdCard_GetPeopleName(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_NAME_FAILURE;
			break;
		}
		_people_name.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_people_sex.clear();
		ret = _pfnIdCard_GetPeopleSex(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_SEX_FAILURE;
			break;
		}
		_people_sex.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_people_nation.clear();
		ret = _pfnIdCard_GetPeopleNation(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_NATION_FAILURE;
			break;
		}
		_people_nation.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_people_birthday.clear();
		ret = _pfnIdCard_GetPeopleBirthday(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_BIRTHDAY_FAILURE;
			break;
		}
		_people_birthday.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_people_address.clear();
		ret = _pfnIdCard_GetPeopleAddress(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_ADDRESS_FAILURE;
			break;
		}
		_people_address.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_people_id_code.clear();
		ret = _pfnIdCard_GetPeopleIdCode(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_PEOPLE_ID_CODE_FAILURE;
			break;
		}
		_people_id_code.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_sign_department.clear();
		ret = _pfnIdCard_GetSignDepartment(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_SIGN_DEPARTMENT_FAILURE;
			break;
		}
		_sign_department.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_valid_start_date.clear();
		ret = _pfnIdCard_GetValidStartDate(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_VALID_START_DATE_FAILURE;
			break;
		}
		_valid_start_date.assign((char*)content);

		ZeroMemory(content, MAX_PATH);
		_valid_end_date.clear();
		ret = _pfnIdCard_GetValidEndDate(content, &length);
		if (ret != 1)
		{
			result = IDCARD_ERROR_READ_VALID_END_DATE_FAILURE;
			break;
		}
		_valid_end_date.assign((char*)content);

		if (read_photo)
		{
			ZeroMemory(_bitmap_data, MAX_BITMAP_DATA_SIZE);
			ret = _pfnIdCard_GetBitmapData((unsigned char*)_bitmap_data, &_bitmap_size);
			if (ret != 1)
			{
				result = IDCARD_ERROR_READ_BITMAP_DATA_FAILURE;
				break;
			}
		}

	} while (false);

	return result;
}

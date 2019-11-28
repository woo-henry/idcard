#include <windows.h>
#include <shlwapi.h>
#include <SynPublic.h>
#include <lib_base/lib_base.h>
#include "idcard_error.h"
#include "idcard_module_impl.h"
//////////////////////////////////////////////////////////////////////////
int current_port = 0;
IDCardData idcard_data;
//////////////////////////////////////////////////////////////////////////
int IDCARD_MODULE_SYNJONES_API IdCard_Init(const char* path)
{
	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_Open(int* port)
{
	int result = IDCARD_ERROR_SUCCESS;
	char photo_path[MAX_PATH] = { 0 };

	do 
	{
		current_port = Syn_FindUSBReader();
		if (current_port == 0)
		{
			result = IDCARD_ERROR_NULL_PORT;
			break;
		}
		
		if (!BaseFsGetAppPathA(photo_path, "temp"))
		{
			result = IDCARD_ERROR_GET_APP_PATH;
			break;
		}

		//设置照片路径(0=C:	1=当前路径	2=指定路径)
		Syn_SetPhotoPath(2, photo_path);

		//0=bmp 1=jpg 2=base64, 3=WLT 4=不生成
		Syn_SetPhotoType(0);

		//生成照片文件名 0=tmp 1=姓名 2=身份证号 3=姓名_身份证号
		Syn_SetPhotoName(0); 

		//0=卡中存储的数据	1=解释之后的数据,男、女、未知
		Syn_SetSexType(1);

		//0=卡中存储的数据	1=解释之后的数据 2=解释之后加"族"
		Syn_SetNationType(1);

		//0=YYYYMMDD,1=YYYY年MM月DD日,2=YYYY.MM.DD,3=YYYY-MM-DD,4=YYYY/MM/DD
		Syn_SetBornType(0);

		//0=YYYYMMDD,1=YYYY年MM月DD日,2=YYYY.MM.DD,3=YYYY-MM-DD,4=YYYY/MM/DD
		Syn_SetUserLifeBType(0);

		//0=YYYYMMDD(不转换),1=YYYY年MM月DD日,2=YYYY.MM.DD,3=YYYY-MM-DD,4=YYYY/MM/DD
		Syn_SetUserLifeEType(1, 1);

		result = Syn_OpenPort(current_port);
		
	} while (false);
	
	return result;
}

int IDCARD_MODULE_SYNJONES_API IdCard_Close()
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (PathFileExistsA(idcard_data.PhotoFileName))
	{
		DeleteFileA(idcard_data.PhotoFileName);
	}

	Syn_ClosePort(current_port);
	current_port = 0;

	ZeroMemory(&idcard_data, sizeof(IDCardData));

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_ReadInfo()
{
	int result = IDCARD_ERROR_SUCCESS;
	char photo_name[_MAX_PATH] = { 0 };
	unsigned char iin[4];
	unsigned char sn[8];

	do 
	{
		if (current_port == 0)
		{
			result = IDCARD_ERROR_NULL_PORT;
			break;
		}

		result = Syn_SetMaxRFByte(current_port, 80, 0);
		if (result != IDCARD_ERROR_SUCCESS)
			break;

		result = Syn_StartFindIDCard(current_port, iin, 0);
		if (result != IDCARD_ERROR_SUCCESS)
			break;

		result = Syn_SelectIDCard(current_port, sn, 0);
		if (result != IDCARD_ERROR_SUCCESS)
			break;

		ZeroMemory(&idcard_data, sizeof(IDCardData));
		result = Syn_ReadFPMsg(current_port, 0, &idcard_data, photo_name);
		if (result == 0 || result == 1)
		{
			result = IDCARD_ERROR_SUCCESS;
		}

	} while (false);

	return result;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleName(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.Name);
	}
	
	if (length)
	{
		*length = lstrlenA(idcard_data.Name);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleSex(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.Sex);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.Sex);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleNation(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.Nation);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.Nation);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleBirthday(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.Born);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.Born);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleAddress(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.Address);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.Address);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetPeopleIdCode(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.IDCardNo);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.IDCardNo);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetSignDepartment(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.GrantDept);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.GrantDept);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetValidStartDate(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.UserLifeBegin);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.UserLifeBegin);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetValidEndDate(unsigned char* content, int* length)
{
	if (current_port == 0)
		return IDCARD_ERROR_NULL_PORT;

	if (content)
	{
		lstrcpyA((char*)content, idcard_data.UserLifeEnd);
	}

	if (length)
	{
		*length = lstrlenA(idcard_data.UserLifeEnd);
	}

	return IDCARD_ERROR_SUCCESS;
}

int IDCARD_MODULE_SYNJONES_API IdCard_GetBitmapData(unsigned char* content, int* length)
{
	int result = IDCARD_ERROR_SUCCESS;
	HANDLE file_handle = INVALID_HANDLE_VALUE;

	do 
	{
		if (current_port == 0)
		{
			result = IDCARD_ERROR_NULL_PORT;
			break;
		}

		file_handle = ::CreateFileA(
			idcard_data.PhotoFileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (file_handle == INVALID_HANDLE_VALUE)
			break;

		LARGE_INTEGER file_size = { 0 };
		if (!::GetFileSizeEx(file_handle, &file_size))
			break;

		if (content)
		{
			unsigned long number_of_bytes_read = 0;
			if (!::ReadFile(file_handle, content, (unsigned long)file_size.QuadPart, &number_of_bytes_read, nullptr))
				break;
		}
		
		if (length)
		{
			*length = (int)file_size.QuadPart;
		}

	} while (false);
	
	if (file_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(file_handle);
		file_handle = INVALID_HANDLE_VALUE;
	}

	return result;
}
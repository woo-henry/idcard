#include <shlwapi.h>
#include "idcard_error.h"
#include "idcard_memory.h"
#include "idcard_module_impl.h"
//////////////////////////////////////////////////////////////////////////
typedef int(PASCAL* lpCVR_InitComm)(int port);
typedef int(PASCAL* lpCVR_CloseComm)();
typedef int(PASCAL* lpCVR_Authenticate)();
typedef int(PASCAL* lpCVR_Read_Content)();
typedef int(PASCAL* lpGetPeopleName)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleSex)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleNation)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleBirthday)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleAddress)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleIDCode)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetDepartment)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetStartDate)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetEndDate)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetNationCode)(UCHAR* sexData, int* pLen);
typedef int(PASCAL* lpGetNationCodeStrU)(unsigned char* nationData, int* pLen);
typedef int(PASCAL* lpCVR_GetSAMIDU)(unsigned char* SAMID);
typedef int(PASCAL* lpGetPeopleChineseName)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPeopleCertVersion)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetNewAppMsgU)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetCertTypeU)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetBMPData)(UCHAR* bmpdate, int* pLen);
typedef int(PASCAL* lpGetFPDate)(UCHAR* strTmp, int* pLen);
typedef int(PASCAL* lpGetPassCheckID)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetPassCheckIDU)(unsigned char* strTmp, int* strLen);
typedef int(PASCAL* lpGetIssuesNum)(int* IssuesNum);
typedef int(PASCAL* lpGetIssuesNumU)(int* IssuesNum);
//////////////////////////////////////////////////////////////////////////
HMODULE module_handle = nullptr;
//////////////////////////////////////////////////////////////////////////
int IDCARD_MODULE_CHINAVISION_API IdCard_Init(const char* path)
{
	int result = IDCARD_ERROR_SUCCESS;
	char* module_path = nullptr;
	
	do 
	{
		if(module_handle != nullptr)
			break;

		module_path = (char*)IdCard_MemAlloc(MAX_PATH);
		if (module_path == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		if (PathCombineA(module_path, path, "termb.dll") == nullptr)
		{
			result = GetLastError();
			break;
		}

		module_handle = LoadLibraryA(module_path);
		if (module_handle == nullptr)
		{
			result = GetLastError();
			break;
		}

	} while (false);

	if (module_path)
	{
		IdCard_MemFree(module_path);
		module_path = nullptr;
	}

	return result;
}

int IDCARD_MODULE_CHINAVISION_API IdCard_Open(int* port)
{
	int result = IDCARD_ERROR_SUCCESS;

	do 
	{
		if (module_handle == nullptr)
		{
			result = IDCARD_ERROR_NULL_INSTANCE;
			break;
		}

		lpCVR_InitComm CVR_InitComm = (lpCVR_InitComm)GetProcAddress(module_handle, "CVR_InitComm");
		if (CVR_InitComm == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		int open_port = 0;
		int cvr_result = 0;
		for (int i = 1001; i <= 1016; i++)
		{
			cvr_result = CVR_InitComm(i);
			if (cvr_result == 1)
			{
				open_port = i;
				break;
			}
		}

		if (cvr_result != 1)
		{
			for (int i = 1; i <= 16; i++)
			{
				int cvr_result = CVR_InitComm(i);
				if (cvr_result == 1)
				{
					open_port = i;
					break;
				}
			}
		}

		lpCVR_GetSAMIDU CVR_GetSAMID = (lpCVR_GetSAMIDU)GetProcAddress(module_handle, "CVR_GetSAMIDU");
		if (CVR_GetSAMID == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		lpCVR_Authenticate CVR_Authenticate = (lpCVR_Authenticate)GetProcAddress(module_handle, "CVR_Authenticate");
		if (CVR_Authenticate == nullptr)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}

		cvr_result = CVR_Authenticate();
		/*
		if (cvr_result != 1)
		{
			result = IDCARD_ERROR_NULL_FUNCTION;
			break;
		}
		*/
	} while (false);
	
	return result;
}

int IDCARD_MODULE_CHINAVISION_API IdCard_Close()
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpCVR_CloseComm CVR_CloseComm = (lpCVR_CloseComm)GetProcAddress(module_handle, "CVR_CloseComm");
	if (CVR_CloseComm == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_CloseComm();
}

int IDCARD_MODULE_CHINAVISION_API IdCard_ReadInfo()
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpCVR_Read_Content CVR_Read_Content = (lpCVR_Read_Content)GetProcAddress(module_handle, "CVR_Read_FPContent");
	if (CVR_Read_Content == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_Read_Content();
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleName(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleName CVR_GetPeopleName = (lpGetPeopleName)GetProcAddress(module_handle, "GetPeopleName");
	if (CVR_GetPeopleName == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleName(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleSex(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleSex CVR_GetPeopleSex = (lpGetPeopleSex)GetProcAddress(module_handle, "GetPeopleSex");
	if (CVR_GetPeopleSex == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleSex(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleNation(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleNation CVR_GetPeopleNation = (lpGetPeopleNation)GetProcAddress(module_handle, "GetPeopleNation");
	if (CVR_GetPeopleNation == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleNation(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleBirthday(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleBirthday CVR_GetPeopleBirthday = (lpGetPeopleBirthday)GetProcAddress(module_handle, "GetPeopleBirthday");
	if (CVR_GetPeopleBirthday == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleBirthday(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleAddress(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleAddress CVR_GetPeopleAddress = (lpGetPeopleAddress)GetProcAddress(module_handle, "GetPeopleAddress");
	if (CVR_GetPeopleAddress == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleAddress(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleIdCode(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetPeopleIDCode CVR_GetPeopleIDCode = (lpGetPeopleIDCode)GetProcAddress(module_handle, "GetPeopleIDCode");
	if (CVR_GetPeopleIDCode == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetPeopleIDCode(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetSignDepartment(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetDepartment CVR_GetDepartment = (lpGetDepartment)GetProcAddress(module_handle, "GetDepartment");
	if (CVR_GetDepartment == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetDepartment(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetValidStartDate(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetStartDate CVR_GetStartDate = (lpGetStartDate)GetProcAddress(module_handle, "GetStartDate");
	if (CVR_GetStartDate == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetStartDate(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetValidEndDate(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetEndDate CVR_GetEndDate = (lpGetEndDate)GetProcAddress(module_handle, "GetEndDate");
	if (CVR_GetEndDate == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetEndDate(content, length);
}

int IDCARD_MODULE_CHINAVISION_API IdCard_GetBitmapData(unsigned char* content, int* length)
{
	if (module_handle == nullptr)
		return IDCARD_ERROR_NULL_INSTANCE;

	lpGetEndDate CVR_GetBmpData = (lpGetEndDate)GetProcAddress(module_handle, "GetBMPData");
	if (CVR_GetBmpData == nullptr)
		return IDCARD_ERROR_NULL_FUNCTION;

	return CVR_GetBmpData(content, length);
}
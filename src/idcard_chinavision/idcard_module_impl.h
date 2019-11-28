#ifndef _IDCARD_MODULE_IMPL_H_
#define _IDCARD_MODULE_IMPL_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "idcard_module_config.h"

#ifdef __cplusplus
_CRT_BEGIN_C_HEADER
#endif

int IDCARD_MODULE_CHINAVISION_API IdCard_Init(const char* path);
int IDCARD_MODULE_CHINAVISION_API IdCard_Open(int* port);
int IDCARD_MODULE_CHINAVISION_API IdCard_Close();
int IDCARD_MODULE_CHINAVISION_API IdCard_ReadInfo();
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleName(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleSex(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleNation(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleBirthday(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleAddress(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetPeopleIdCode(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetSignDepartment(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetValidStartDate(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetValidEndDate(unsigned char* content, int* length);
int IDCARD_MODULE_CHINAVISION_API IdCard_GetBitmapData(unsigned char* content, int* length);

#ifdef __cplusplus
_CRT_END_C_HEADER
#endif

#endif _IDCARD_MODULE_IMPL_H_
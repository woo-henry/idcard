#ifndef _IDCARD_DEFINE_H_
#define _IDCARD_DEFINE_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <wtypes.h>

typedef enum
{
	ManufactorType_Unknown			= 0,
	ManufactorType_ChinaVision		= 1,		// 华视
	ManufactorType_Synjones			= 2,		// 新中新
} IdCardManufactorType;

typedef enum
{
	CertType_Unknown				= 0,
	CertType_China					= 1,
	CertType_HAT					= 2,
	CertType_Foreign				= 3
} IdCardCertType;

#ifdef __cplusplus
_CRT_BEGIN_C_HEADER
#endif

typedef int (* IdCard_Init)(const char* path);
typedef int ( *IdCard_Open)(int* port);
typedef int (* IdCard_Close)();
typedef int (* IdCard_ReadInfo)();
typedef int (* IdCard_GetPeopleName)(unsigned char* content, int* length);
typedef int (* IdCard_GetPeopleSex)(unsigned char* content, int* length);
typedef int (* IdCard_GetPeopleNation)(unsigned char* content, int* length);
typedef int (* IdCard_GetPeopleBirthday)(unsigned char* content, int* length);
typedef int (* IdCard_GetPeopleAddress)(unsigned char* content, int* length);
typedef int (* IdCard_GetPeopleIdCode)(unsigned char* content, int* length);
typedef int (* IdCard_GetSignDepartment)(unsigned char* content, int* length);
typedef int (* IdCard_GetValidStartDate)(unsigned char* content, int* length);
typedef int (* IdCard_GetValidEndDate)(unsigned char* content, int* length);
typedef int (* IdCard_GetBitmapData)(unsigned char* content, int* length);

#ifdef __cplusplus
_CRT_END_C_HEADER
#endif

#endif // _IDCARD_DEFINE_H_
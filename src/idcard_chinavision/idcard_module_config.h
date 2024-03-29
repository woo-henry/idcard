#ifndef _IDCARD_MODULE_CONFIG_H_
#define _IDCARD_MODULE_CONFIG_H_

//////////////////////////////////////////////////////////////////////////
#if defined(IDCARD_MODULE_CHINAVISION_STATIC)
#define IDCARD_MODULE_CHINAVISION_API
#elif defined(IDCARD_MODULE_CHINAVISION_DYNAMIC)
//  definitions used when building DLL
#define IDCARD_MODULE_CHINAVISION_API __declspec(dllexport)
#else
//  definitions used when using DLL
#define IDCARD_MODULE_CHINAVISION_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
#include <wtypes.h>

#endif // _IDCARD_MODULE_CONFIG_H_

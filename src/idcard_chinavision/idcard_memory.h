#ifndef _IDCARD_MEMORY_H_
#define _IDCARD_MEMORY_H_
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
_CRT_BEGIN_C_HEADER
#endif

PVOID IdCard_MemAlloc(size_t size);
PVOID IdCard_MemRealloc(void* memory, size_t size);
bool IdCard_MemFree(void* memory);
bool IdCard_MemCheck();

#ifdef __cplusplus
_CRT_END_C_HEADER
#endif

#endif // _IDCARD_MEMORY_H_
#include <stdio.h>
#include <windows.h>
#include "idcard_memory.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	size_t	size;
	char* memory;
}BASE_MEMORY_OBJECT, * PBASE_MEMORY_OBJECT;

static volatile long memory_reference_count = 0;

PVOID IdCard_MemAlloc(size_t size)
{
	size_t memory_size = size + sizeof(BASE_MEMORY_OBJECT);
	PBASE_MEMORY_OBJECT memory_obj = (PBASE_MEMORY_OBJECT)::VirtualAlloc(nullptr, memory_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (memory_obj == nullptr)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return nullptr;
	}

	memory_obj->size = size;
	memory_obj->memory = (char*)memory_obj;

	InterlockedIncrement(&memory_reference_count);
#ifdef _DEBUG
	char msg[96] = { 0 };
	snprintf(msg, 96, "BaseMemAlloc : memory alloc [%ld] bytes at address [%p], memory reference [%ld]\r\n", memory_obj->size, memory_obj->memory, ReadAcquire(&memory_reference_count));
	OutputDebugStringA(msg);
#endif

	return (PVOID)((char*)memory_obj + sizeof(BASE_MEMORY_OBJECT));
}

PVOID IdCard_MemRealloc(void* memory, size_t size)
{
	if (!IdCard_MemFree(memory))
		return nullptr;

	return IdCard_MemAlloc(size);
}

bool IdCard_MemFree(void* memory)
{
	if (memory == nullptr)
	{
		SetLastError(ERROR_BAD_ARGUMENTS);
		return false;
	}

	char* memory_ptr = (char*)memory;
	PBASE_MEMORY_OBJECT memory_obj = (PBASE_MEMORY_OBJECT)(memory_ptr - sizeof(BASE_MEMORY_OBJECT));

#ifdef _DEBUG
	char* memory_address = (char*)memory_obj->memory;
	size_t memory_size = memory_obj->size;
#endif

	bool result = ::VirtualFree(memory, 0, MEM_RELEASE);
	if (result)
	{
#ifdef _DEBUG
		char msg[96] = { 0 };
		snprintf(msg, 96, "BaseMemFree : memory free [%ld] bytes at address [%p], memory reference [%ld]\r\n", memory_size, memory_address, ReadAcquire(&memory_reference_count));
		OutputDebugStringA(msg);
#endif
		InterlockedDecrement(&memory_reference_count);
	}
	else
	{
		SetLastError(GetLastError());
	}

	return result;
}

bool IdCard_MemCheck()
{
	return ReadAcquire(&memory_reference_count) == 0;
}
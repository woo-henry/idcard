#ifndef _IDCARD_MODULE_H_
#define _IDCARD_MODULE_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <string>
#include "idcard_config.h"
#include "idcard_define.h"

#define MAX_BITMAP_DATA_SIZE	40960

class IdCardModule
{
public:
	IdCardModule(const IdCardConfigItem* config_item);
protected:
	virtual ~IdCardModule();
public:
	virtual int Init();
	virtual void Dispose();
	virtual int Open(int* port);
	virtual int Close();
	virtual int ReadCardInfo(bool read_photo = true);
	const IdCardConfigItem* GetConfigItem() { return _config_item; }
	const char* GetPeopleName() { return _people_name.c_str(); }
	const char* GetPeopleSex() { return _people_sex.c_str(); }
	const char* GetPeopleNation() { return _people_nation.c_str(); }
	const char* GetPeopleBirthday() { return _people_birthday.c_str(); }
	const char* GetPeopleAddress() { return _people_address.c_str(); }
	const char* GetPeopleIdCode() { return _people_id_code.c_str(); }
	const char* GetSignDepartment() { return _sign_department.c_str(); }
	const char* GetValidStartDate() { return _valid_start_date.c_str(); }
	const char* GetValidEndDate() { return _valid_end_date.c_str(); }
	const char* GetBitmapData() { return _bitmap_data; }
	const int GetBitmapSize() { return _bitmap_size; }
protected:
	const IdCardConfigItem* _config_item;
private:
	HMODULE						_module_handle;
	IdCard_Init					_pfnIdCard_Init;
	IdCard_Open					_pfnIdCard_Open;
	IdCard_Close				_pfnIdCard_Close;
	IdCard_ReadInfo				_pfnIdCard_ReadInfo;
	IdCard_GetPeopleName		_pfnIdCard_GetPeopleName;
	IdCard_GetPeopleSex			_pfnIdCard_GetPeopleSex;
	IdCard_GetPeopleNation		_pfnIdCard_GetPeopleNation;
	IdCard_GetPeopleBirthday	_pfnIdCard_GetPeopleBirthday;
	IdCard_GetPeopleAddress		_pfnIdCard_GetPeopleAddress;
	IdCard_GetPeopleIdCode		_pfnIdCard_GetPeopleIdCode;
	IdCard_GetSignDepartment	_pfnIdCard_GetSignDepartment;
	IdCard_GetValidStartDate	_pfnIdCard_GetValidStartDate;
	IdCard_GetValidEndDate		_pfnIdCard_GetValidEndDate;
	IdCard_GetBitmapData		_pfnIdCard_GetBitmapData;
private:
	std::string _people_name;
	std::string _people_sex;
	std::string _people_nation;
	std::string _people_birthday;
	std::string _people_address;
	std::string _people_id_code;
	std::string _sign_department;
	std::string _valid_start_date;
	std::string _valid_end_date;
	char _bitmap_data[MAX_BITMAP_DATA_SIZE * 2];
	int _bitmap_size;
};

#endif _IDCARD_MODULE_H_
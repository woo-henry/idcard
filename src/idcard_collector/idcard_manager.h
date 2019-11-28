#ifndef _IDCARD_MANAGER_H_
#define _IDCARD_MANAGER_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include "idcard_config.h"
#include "idcard_module.h"

class IdCardData
{
public:
	IdCardData(const char* people_name,
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
		: _people_name(people_name)
		, _people_sex(people_sex)
		, _people_nation(people_nation)
		, _people_birthday(people_birthday)
		, _people_address(people_address)
		, _people_id_code(people_id_code)
		, _sign_department(sign_department)
		, _valid_start_date(valid_start_date)
		, _valid_end_date(valid_end_date)
		, _bitmap_size(bitmap_size)
	{
		ZeroMemory(_bitmap_data, MAX_BITMAP_DATA_SIZE * 2);
		CopyMemory(_bitmap_data, bitmap_data, bitmap_size);
	}
public:
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

class IdCardReadCallback
{
public:
	virtual void OnReadCompleted(IdCardData* data) = 0;
	virtual void OnReadError(int error) = 0;
};

class IdCardManager
{
public:
	static boost::shared_ptr<IdCardManager>& GetInstance();
protected:
	IdCardManager();
public:
	virtual ~IdCardManager();
public:
	int Init(boost::shared_ptr<IdCardConfig>& config);
	void Dispose();
	int ReadCard(bool read_photo, IdCardReadCallback* callback);
	int OutputFile(const char* output_file,
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
		int bitmap_size);
	int OutputNetwork(const char* output_url,
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
		int bitmap_size);
private:
	std::vector<IdCardModule*> _idcard_modules;
};

#endif _IDCARD_MANAGER_H_
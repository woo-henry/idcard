#ifndef _IDCARD_MANAGER_H_
#define _IDCARD_MANAGER_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include "idcard_config.h"
#include "idcard_module.h"

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
	int ReadCard(const char* output_target, const char* encoding, bool read_photo = true);
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
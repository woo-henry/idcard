#ifndef _IDCARD_CONFIG_H_
#define _IDCARD_CONFIG_H_
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class IdCardConfigItem
{
public:
	IdCardConfigItem(const char* manufactor_name, const char* module_name, const char* module_path)
		: _manufactor_name(manufactor_name)
		, _module_name(module_name)
		, _module_path(module_path)
	{

	}
	virtual ~IdCardConfigItem(){}
public:
	const char* GetManufactorName() const { return _manufactor_name.c_str(); }
	const char* GetModuleName() const { return _module_name.c_str(); }
	const char* GetModulePath() const { return _module_path.c_str(); }
private:
	std::string		_manufactor_name;
	std::string		_module_name;
	std::string		_module_path;
};

class IdCardConfig
{
public:
	static boost::shared_ptr<IdCardConfig>& GetInstance();
protected:
	IdCardConfig();
public:
	virtual ~IdCardConfig();
public:
	// load the specified configuration file
	int Load(const char* config_file);
	// service config
	int GetIdCardCount() const { return (int)_id_cards.size(); }
	const IdCardConfigItem* GetIdCard(int index);
	const char* GetOutputTarget() const { return _output_target.c_str(); }
	const char* GetEncoding() const { return _encoding.c_str(); }
	unsigned int GetTimerSecond() const { return _timer_second; }
	bool IsTest() const { return _test; }
private:
	std::vector<IdCardConfigItem*>	_id_cards;
	std::string						_output_target;
	std::string						_encoding;
	unsigned int					_timer_second;
	bool							_test;
};

#endif _IDCARD_CONFIG_H_
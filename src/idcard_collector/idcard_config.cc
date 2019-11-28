#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#include <tinyxml2.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include "lib_base/lib_base.h"
#include "idcard_string_util.h"
#include "idcard_error.h"
#include "idcard_config.h"
//////////////////////////////////////////////////////////////////////////
// SyncConfig
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define CONFIG_SERVICE_DIR				"..\\conf"
#else
#define CONFIG_SERVICE_DIR				"conf"
#endif
#define CONFIG_SERVICE_FILE				"idcard.xml"
//////////////////////////////////////////////////////////////////////////
static boost::shared_ptr<IdCardConfig> _instance = nullptr;
static boost::mutex _instance_mutex;
boost::shared_ptr<IdCardConfig>& IdCardConfig::GetInstance()
{
	if (_instance == nullptr)
	{
		boost::lock_guard<boost::mutex> lock(_instance_mutex);
		if (_instance == nullptr)
		{
			_instance.reset(new IdCardConfig);
		}
	}

	return _instance;
}

//////////////////////////////////////////////////////////////////////////
IdCardConfig::IdCardConfig()
{

}

IdCardConfig::~IdCardConfig()
{
	if (!_id_cards.empty())
	{
		for (auto it = _id_cards.begin(); it != _id_cards.end(); it++)
		{
			IdCardConfigItem* idcard = *it;
			if (idcard)
			{
				delete idcard;
				idcard = nullptr;
			}
		}

		_id_cards.erase(_id_cards.begin(), _id_cards.end());
	}
}

int IdCardConfig::Load(const char* config_file)
{
	int result = IDCARD_ERROR_SUCCESS;
	char* config_filename = nullptr;

	do
	{
		BaseLogInfo("IdCardCollector loading configuration ...");

		config_filename = (char*)BaseMemAlloc(MAX_PATH);
		if (config_filename == nullptr)
		{
			result = IDCARD_ERROR_MEMORY_ALLOCATE;
			break;
		}

		if (config_file == nullptr)
		{
			if (!BaseFsGetAppPathA(config_filename, CONFIG_SERVICE_DIR))
			{
				result = IDCARD_ERROR_GET_APP_PATH;
				break;
			}

			if (::PathAddBackslashA(config_filename) == nullptr)
			{
				result = IDCARD_ERROR_PATH_ADD_BACKSLASH;
				break;
			}

			::lstrcatA(config_filename, CONFIG_SERVICE_FILE);
		}
		else
		{
			::lstrcpyA(config_filename, config_file);
		}

		tinyxml2::XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
		tinyxml2::XMLError error = doc.LoadFile(config_filename);
		if (error != tinyxml2::XML_SUCCESS)
		{
			BaseLogError("IdCardCollector failed to load configuration file[%s], error = %d", config_filename, error);
			result = IDCARD_ERROR_XML_LOAD_FAILURE;
			break;
		}

		tinyxml2::XMLElement* ele_config = doc.FirstChildElement("config");
		if (ele_config == nullptr)
		{
			BaseLogError("IdCardCollector failed to load configuration file, the root node does not exist");
			result = IDCARD_ERROR_XML_FIND_ELEMENT;
			break;
		}

		const char* output_target = ele_config->Attribute("output_target");
		if (output_target == nullptr)
		{
			BaseLogError("IdCardCollector failed to load configuration file, the output_target attribute does not exist");
			result = IDCARD_ERROR_XML_FIND_ATTRIBUTE;
			break;
		}

		const char* encoding = ele_config->Attribute("encoding");
		if (encoding == nullptr)
		{
			BaseLogError("IdCardCollector failed to load configuration file, the encoding attribute does not exist");
			result = IDCARD_ERROR_XML_FIND_ATTRIBUTE;
			break;
		}

		unsigned int timer_second = ele_config->UnsignedAttribute("timer_second", 2);
		bool test = ele_config->BoolAttribute("test", false);

		// id_card
		tinyxml2::XMLElement* element_idcard = ele_config->FirstChildElement("id_card");
		if (element_idcard == nullptr)
		{
			BaseLogError("IdCardCollector failed to load configuration file, the id_card node does not exist");
			result = IDCARD_ERROR_XML_FIND_ELEMENT;
			break;
		}

		while (element_idcard)
		{
			const char* manufactor_name = element_idcard->Attribute("name");
			if (manufactor_name == nullptr)
			{
				BaseLogError("IdCardCollector failed to load configuration file, the name attribute does not exist");
				result = IDCARD_ERROR_XML_FIND_ATTRIBUTE;
				break;
			}

			const char* module_name = element_idcard->Attribute("module");
			if (module_name == nullptr)
			{
				BaseLogError("IdCardCollector failed to load configuration file, the module attribute does not exist");
				result = IDCARD_ERROR_XML_FIND_ATTRIBUTE;
				break;
			}

			const char* module_path = element_idcard->Attribute("path");
			if (module_path == nullptr)
			{
				BaseLogError("IdCardCollector failed to load configuration file, the path attribute does not exist");
				result = IDCARD_ERROR_XML_FIND_ATTRIBUTE;
				break;
			}

			std::string manufactor_name_string = StrUTF82A(manufactor_name);
			std::string module_name_string = StrUTF82A(module_name);
			std::string module_path_string = StrUTF82A(module_path);
			_id_cards.push_back(new IdCardConfigItem(manufactor_name_string.c_str(), module_name_string.c_str(), module_path_string.c_str()));

			element_idcard = element_idcard->NextSiblingElement("id_card");
		}

		if(result != IDCARD_ERROR_SUCCESS)
			break;

		_output_target.assign(output_target);
		_encoding.assign(encoding);
		_timer_second = timer_second;
		_test = test;

		BaseLogInfo("IdCardCollector loading configuration successful");

	} while (false);

	if (config_filename)
	{
		BaseMemFree(config_filename);
		config_filename = nullptr;
	}

	return result;
}

const IdCardConfigItem* IdCardConfig::GetIdCard(int index)
{
	int position = 0;
	for (auto it = _id_cards.begin(); it != _id_cards.end(); it++)
	{
		IdCardConfigItem* idcard = *it;
		if (idcard && index == position)
			return idcard;

		position++;
	}

	return nullptr;
}
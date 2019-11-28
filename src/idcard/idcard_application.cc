#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <lib_base/lib_base.h>
#include "idcard_error.h"
#include "idcard_config.h"
#include "idcard_manager.h"
//////////////////////////////////////////////////////////////////////////
int idcard_test(const char* test_filename, const char* output_url, const char* encoding)
{
	return 0;
}

int main(int argc, char** argv)
{
	int result = IDCARD_ERROR_SUCCESS;

	do 
	{
		if (!BaseLogInitialize())
		{
			result = IDCARD_ERROR_LOG_INIT_FAILURE;
			break;
		}

		boost::shared_ptr<IdCardConfig> idcard_config = IdCardConfig::GetInstance();
		if (idcard_config == nullptr)
		{
			result = IDCARD_ERROR_NULL_INSTANCE;
			BaseLogError("IdCard init config failed : %d", result);
			break;
		}

		result = idcard_config->Load(nullptr);
		if (result != IDCARD_ERROR_SUCCESS)
		{
			BaseLogError("IdCard load config failed : %d", result);
			break;
		}

		boost::shared_ptr<IdCardManager> idcard_manager = IdCardManager::GetInstance();
		if (idcard_manager == nullptr)
		{
			result = IDCARD_ERROR_NULL_INSTANCE;
			BaseLogError("IdCard init config failed : %d", result);
			break;
		}

		result = idcard_manager->Init(idcard_config);
		if (result != IDCARD_ERROR_SUCCESS)
		{
			BaseLogError("IdCard manager init failed : %d", result);
			break;
		}

		if (idcard_config->IsTest())
		{
			char test_filename[MAX_PATH] = { 0 };
#ifdef _DEBUG
			if (BaseFsGetAppPathA(test_filename, "..\\test\\test.jpg"))
#else
			if (BaseFsGetAppPathA(test_filename, "test\\test.jpg"))
#endif
			{
				result = idcard_test(test_filename, IdCardConfig::GetInstance()->GetOutputTarget(), IdCardConfig::GetInstance()->GetEncoding());
				BaseLogInfo("IdCard test result : %d", result);
			}
			else
			{
				BaseLogWarn("IdCard test error");
			}
			break;
		}
		else
		{
			result = idcard_manager->ReadCard(idcard_config->GetOutputTarget(), idcard_config->GetEncoding());
		}

		if (result != IDCARD_ERROR_SUCCESS)
		{
			BaseLogError("IdCard manager read card failed : %d", result);
			break;
		}

		BaseLogInfo("IdCard manager read card successful");

	} while (false);

	BaseLogShutdown();

	return result;
}
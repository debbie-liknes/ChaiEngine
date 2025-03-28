#include <Plugin/ServiceLocator.h>

namespace chai
{
	ServiceLocator& ServiceLocator::getInstance()
	{
		static ServiceLocator locator;
		return locator;
	}

	void ServiceLocator::Shutdown()
	{
		m_services.clear();
	}
}
#pragma once

#include <Action/Action.h>

#include <Containers/Dictionary.h>
#include <memory>

namespace chai
{
	class ActionManager
	{
    public:

	private:
        Dictionary<std::shared_ptr<Action>> actionDict_;
	};
}

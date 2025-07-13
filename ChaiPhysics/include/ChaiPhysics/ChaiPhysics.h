#pragma once

#include <thread>
#include <atomic>

namespace chai
{
	class ChaiPhysics
	{
	public:
		ChaiPhysics();
		void init();

		~ChaiPhysics();

	protected:
		bool isDone() const;
		void run();

	private:
		std::thread m_thread;
		std::atomic<bool> m_done = false;
	};
}

#include "thread_pool.h"

thread_pool::thread_pool(size_t n)
{
	start_threads(n);
}

thread_pool::thread_pool(size_t n, bool enable)
{
	start_threads(n);
}

thread_pool::~thread_pool()
{
	finish();
}

bool thread_pool::is_empty()
{
	return tasks.empty();
}

void thread_pool::thread_task()
{
	for (;;)
	{
		std::packaged_task<void()> func;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			if (tasks.empty())
			{
				tasks_cond_var.wait(lock, [&] { return !tasks.empty(); });
			}
			func = std::move(tasks.front());
			tasks.pop_front();
		}
		if (!func.valid())
		{
			return;
		}

		func();

	}
}

void thread_pool::start_threads(size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		future_results.push_back(
			std::async(
				std::launch::async,
				[this] { thread_task(); }
			)
		);
	}
}

void thread_pool::abort()
{
	cancel_pending();
	finish();
}

void thread_pool::cancel_pending()
{
	std::unique_lock<std::mutex> lock(queue_mutex);
	tasks.clear();
}

void thread_pool::finish()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		for (auto && _ : future_results)
		{
			tasks.push_back({});
		}
	}
	tasks_cond_var.notify_all();
	future_results.clear();
}
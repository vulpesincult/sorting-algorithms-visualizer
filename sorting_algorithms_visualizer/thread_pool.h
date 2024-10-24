#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <future>

class thread_pool
{
private:
	std::list<std::packaged_task<void()>> tasks;
	std::vector<std::future<void>> future_results;
	std::mutex queue_mutex;
	std::condition_variable tasks_cond_var;

	void thread_task();

public:

	thread_pool(size_t n);
	thread_pool(size_t n, bool);
	~thread_pool();
	bool is_empty();
	void start_threads(size_t n);
	void abort();
	void cancel_pending();
	void finish();

	template<class func_t, class result_t = typename std::result_of<func_t & ()>::type>
	std::future<result_t> add_task(func_t && f)
	{
		std::packaged_task<result_t()> package(std::forward<func_t>(f));
		auto result = package.get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			tasks.emplace_back(std::move(package));
		}
		tasks_cond_var.notify_one();

		return result;
	}
};
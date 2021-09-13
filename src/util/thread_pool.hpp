#pragma once

#include <logger.hpp>
#include <include/external.hpp>

class ThreadPool {

	public:

		using Task = std::function<void()>;

		ThreadPool(size_t count = ThreadPool::optimal());
		~ThreadPool();

		void enqueue(const Task& task);

		template< typename Func, typename Arg, typename... Args >
		void enqueue( Func func, Arg arg, Args... args ) {
			this->enqueue( std::bind(func, arg, args...) );
		}

		static size_t optimal();

	private:

		bool stop;
	
		std::vector<std::thread> workers;
		std::mutex queue_mutex;
		std::queue<Task> tasks;
		std::condition_variable condition;

		void run();

};


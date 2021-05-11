
#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>

// Author: https://github.com/progschj/ThreadPool, modified by magistermaks

class ThreadPool {

	public:
		ThreadPool(size_t);
		ThreadPool();
		~ThreadPool();

		void enqueue(std::function<void(void*)> task);
		static int optimal();

		template<class T> 
		int addWorker( std::function<T()> );

	private:
		std::vector< std::thread > workers;
		std::queue< std::function<void(void*)> > tasks;
    
		// synchronization
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;
};

template<class T> 
int ThreadPool::addWorker( std::function<T()> init ) {
	
	workers.emplace_back( [this, init] {

		T arg = init();

		while(true) {

			std::function<void(void*)> task;

			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);

				this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
				if(this->stop && this->tasks.empty()) return;

				task = std::move( this->tasks.front() );
				this->tasks.pop();
			}

			task( (void*) &arg );

		}
	} );

	return workers.size() - 1;

}



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
		~ThreadPool();

		int addWorker();
		void enqueue(std::function<void()> task);
		static int optimal();

	private:
		std::vector< std::thread > workers;
		std::queue< std::function<void()> > tasks;
    
		// synchronization
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;
};


#include "thread_pool.hpp"
 
// the constructor launches workers
ThreadPool::ThreadPool(size_t threads) : stop(false) {

	while( threads --> 0 ) {
		addWorker();
	}

}

// add new worker to the pool
int ThreadPool::addWorker() {
	
	workers.emplace_back( [this] {
		while(true) {

			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);

				this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
				if(this->stop && this->tasks.empty()) return;

				task = std::move( this->tasks.front() );
				this->tasks.pop();
			}

			task();

		}
	} );

	return workers.size() - 1;

}

// add new work item to the pool
void ThreadPool::enqueue(std::function<void()> task) {
        
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if( stop ) throw std::runtime_error("Unable to add task to a stopped pool!");

        tasks.emplace(task);
    }
    condition.notify_one();

}

// the destructor joins all threads
ThreadPool::~ThreadPool() {

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();

    for(auto& worker : workers) {
        worker.join();
	}

}

// get the optimal amount of workers
int ThreadPool::optimal() {
	return std::max((int) std::thread::hardware_concurrency() - 1, 1);
}


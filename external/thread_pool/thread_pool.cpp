
#include "thread_pool.hpp"
 
// the constructor launches workers
ThreadPool::ThreadPool( size_t threads ) : stop(false) {

	while( threads --> 0 ) {
		addWorker<int>( []{ return 0; } );
	}

}

// the constructor creates empty pool
ThreadPool::ThreadPool() : stop(false) {

	// add threads using pool.addWorker<>()

}

// add new work item to the pool
void ThreadPool::enqueue(std::function<void(void*)> task) {
        
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


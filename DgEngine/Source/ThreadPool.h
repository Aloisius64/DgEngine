////////////////////////////////////////////////////////////////////////////////
// Filename: ThreadPool.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Thread.h"


class ThreadPool {
public:
	ThreadPool(size_t = std::thread::hardware_concurrency());

	virtual ~ThreadPool();

	template<class F>
	void SubmitTask(F f);

	bool WaitAllTasks();
	
private:
	// Worker threads.
	std::vector<std::thread>			m_Workers;
	// Task queue.
	std::queue<std::function<void()>>	m_Tasks;
	// Synchronization.
	std::mutex							m_QueueMutex;
	std::condition_variable				m_ConditionTaskSubmitted;
	std::condition_variable				m_ConditionAllTasksDone;
	bool								m_Done;
	unsigned int						m_FreeThread;
};

// The constructor just launches some amount of workers.
inline ThreadPool::ThreadPool(size_t threads) {
	m_Done = false;
	m_FreeThread = threads;

	for(size_t i = 0; i<threads; ++i)
		m_Workers.emplace_back(
		[this] {
		while(!this->m_Done) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(this->m_QueueMutex);
				this->m_ConditionTaskSubmitted.wait(lock,
					[this] { return this->m_Done||!this->m_Tasks.empty(); });
				if(this->m_Done && this->m_Tasks.empty())
					return;
				task = std::move(this->m_Tasks.front());
				this->m_Tasks.pop();
				// Decrease the number of free thread.
				m_FreeThread--;
			}
			task();
			{
				std::unique_lock<std::mutex> lock(this->m_QueueMutex);
				// Increase the number of free thread.
				m_FreeThread++;
				// Notification for the completion of the task.
				m_ConditionAllTasksDone.notify_all();
			}
		}
	}
	);
}

template<class F>
void ThreadPool::SubmitTask(F f) {
	std::unique_lock<std::mutex> lock(m_QueueMutex);

	// Don't allow enqueueing after stopping the pool.
	if(m_Done)
		throw std::runtime_error("enqueue on stopped ThreadPool");

	m_Tasks.push(f);
	m_ConditionTaskSubmitted.notify_one();
}

// The destructor joins all threads.
inline ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(m_QueueMutex);
		m_Done = true;
		m_ConditionTaskSubmitted.notify_all();
	}

	for(auto &worker:m_Workers) {
		worker.join();
	}

	m_Workers.clear();
}

// Wait until all tasks are done.
inline bool ThreadPool::WaitAllTasks() {
	{
		std::unique_lock<std::mutex> lock(this->m_QueueMutex);
		this->m_ConditionAllTasksDone.wait(lock, [this] { return this->m_Workers.size()==m_FreeThread; });
	}
	return this->m_Tasks.empty();
}

#pragma once
#include "DAG.h"
#include <mutex>
#include <condition_variable>
#include <atomic>

class Executor {
public:
    explicit Executor(DAG& dag) : dag_(dag) {}
    void run();

private:
    DAG& dag_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> error_occurred_{false};
    int active_jobs_count_ = 0;

    void worker_thread(std::string job_id);
};
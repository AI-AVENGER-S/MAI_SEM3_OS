#include "../include/Executor.h"
#include <thread>
#include <iostream>
#include <cstdlib>

void Executor::worker_thread(std::string job_id) {
    Job& job = dag_.jobs[job_id];
    
    std::cout << "Running " << job_id << "...\n";
    int ret_code = std::system(job.cmd.c_str());

    std::unique_lock<std::mutex> lock(mtx_);
    active_jobs_count_--;

    if (ret_code != 0) {
        std::cerr << "Job " << job_id << " failed!\n";
        error_occurred_ = true;
        job.state = JobState::FAILED;
    } else {
        std::cout << "Job " << job_id << " finished.\n";
        job.state = JobState::COMPLETED;
        
        for (const auto& child_id : job.children) {
            dag_.jobs[child_id].unfinished_parents--;
        }
    }
    
    // уведомляем главный поток, что место освободилось или появились новые готовые задачи
    lock.unlock();
    cv_.notify_all();
}

void Executor::run() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);

        if (error_occurred_) {
            if (active_jobs_count_ == 0) break;
            cv_.wait(lock);
            continue;
        }

        bool all_done = true;
        for (const auto& [id, job] : dag_.jobs) {
            if (job.state != JobState::COMPLETED) {
                all_done = false;
                break;
            }
        }
        if (all_done) break;

        std::vector<std::string> ready_to_run;
        for (const auto& [id, job] : dag_.jobs) {
            if (job.state == JobState::PENDING && job.unfinished_parents == 0) {
                ready_to_run.push_back(id);
            }
        }

        bool launched_any = false;
        for (const auto& id : ready_to_run) {
            if (active_jobs_count_ < dag_.max_concurrent) {
                dag_.jobs[id].state = JobState::RUNNING;
                active_jobs_count_++;
                std::thread(&Executor::worker_thread, this, id).detach();
                launched_any = true;
            } else {
                break;
            }
        }

        cv_.wait(lock);
    }

    if (error_occurred_) std::cout << "Ошибка.\n";
    else std::cout << "Все норм.\n";
}
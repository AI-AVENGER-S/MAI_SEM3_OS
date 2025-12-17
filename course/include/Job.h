#pragma once
#include <string>
#include <vector>
#include <set>

enum class JobState { PENDING, RUNNING, COMPLETED, FAILED };

struct Job {
    std::string id;
    std::string cmd;
    std::vector<std::string> dependencies;
    
    std::vector<std::string> children; // id тех, кто зависит от этой джобы
    int unfinished_parents = 0;
    JobState state = JobState::PENDING;
};
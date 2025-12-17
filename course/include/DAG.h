#pragma once
#include "Job.h"
#include <map>
#include <string>
#include <yaml-cpp/yaml.h>

class DAG {
public:
    std::map<std::string, Job> jobs;
    int max_concurrent = 1;

    void load(const std::string& filename);
    void validate();

private:
    void check_cycles();
    void check_connectivity();
    void check_start_end_nodes();
    
    bool dfs_cycle(const std::string& current, std::map<std::string, int>& visited);
};
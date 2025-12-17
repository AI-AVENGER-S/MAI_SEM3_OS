#include "../include/DAG.h"
#include <iostream>
#include <queue>
#include <stdexcept>

void DAG::load(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename);
    
    if (config["max_concurrent"]) {
        max_concurrent = config["max_concurrent"].as<int>();
    }

    auto jobs_node = config["jobs"];
    for (const auto& j : jobs_node) {
        Job job;
        job.id = j["id"].as<std::string>();
        job.cmd = j["cmd"].as<std::string>();
        if (j["depends_on"]) {
            for (const auto& dep : j["depends_on"]) {
                job.dependencies.push_back(dep.as<std::string>());
            }
        }
        jobs[job.id] = job;
    }
    // построение графа
    for (auto& [id, job] : jobs) {
        job.unfinished_parents = job.dependencies.size();
        for (const auto& parent_id : job.dependencies) {
            if (jobs.find(parent_id) == jobs.end()) {
                throw std::runtime_error("Неизвестная зависимость: " + parent_id);
            }
            jobs[parent_id].children.push_back(id);
        }
    }
}

void DAG::validate() {
    check_start_end_nodes();
    check_cycles();
    check_connectivity();
    std::cout << "[DAG] Validation passed.\n";
}

void DAG::check_start_end_nodes() {
    int start_nodes = 0;
    int end_nodes = 0;
    for (const auto& [id, job] : jobs) {
        if (job.dependencies.empty()) start_nodes++;
        if (job.children.empty()) end_nodes++;
    }
    if (start_nodes == 0) throw std::runtime_error("Нет стратовых нод!");
    if (end_nodes == 0) throw std::runtime_error("Нет конечных нод!");
}

bool DAG::dfs_cycle(const std::string& current, std::map<std::string, int>& visited) {
    visited[current] = 1;
    for (const auto& child_id : jobs[current].children) {
        if (visited[child_id] == 1) return true;
        if (visited[child_id] == 0) {
            if (dfs_cycle(child_id, visited)) return true;
        }
    }
    visited[current] = 2;
    return false;
}

void DAG::check_cycles() {
    std::map<std::string, int> visited; // 0: белый, 1: серый, 2: черный
    for (const auto& [id, job] : jobs) {
        if (visited[id] == 0) {
            if (dfs_cycle(id, visited)) throw std::runtime_error("Есть цикл!");
        }
    }
}

void DAG::check_connectivity() {
    if (jobs.empty()) return;
    
    std::map<std::string, std::vector<std::string>> undirected;
    for (const auto& [id, job] : jobs) {
        for (const auto& child : job.children) {
            undirected[id].push_back(child);
            undirected[child].push_back(id);
        }
    }

    std::set<std::string> visited;
    std::queue<std::string> q;
    q.push(jobs.begin()->first);
    visited.insert(jobs.begin()->first);

    while (!q.empty()) {
        std::string u = q.front();
        q.pop();
        for (const auto& v : undirected[u]) {
            if (visited.find(v) == visited.end()) {
                visited.insert(v);
                q.push(v);
            }
        }
    }

    if (visited.size() != jobs.size()) {
        throw std::runtime_error("Есть несколько компонентов связности!");
    }
}
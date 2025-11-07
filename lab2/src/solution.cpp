#include "../include/solution.hpp"
#include <cstdlib>
#include <ctime>
#include <limits>
#include <stdexcept> 
#include <algorithm> 

using namespace std;

Point& Point::operator+=(const Point& other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
}

Point& Point::operator/=(int count) {
    if (count > 0) {
        this->x /= count;
        this->y /= count;
    }
    return *this;
}

double calculate_distance(const Point& p1, const Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

KMeans::KMeans(int k, int max_threads, const vector<Point>& data)
    : k(k), max_threads(max_threads), points(data) {
    
    if (k <= 0) throw runtime_error("К должно быть больше нуля");
    if (max_threads <= 0) throw runtime_error("Количество потоков должно быть больше нуля");
    if (points.empty()) throw runtime_error("Количество точек должно быть больше нуля");
    if (k > points.size()) throw runtime_error("Количество кластеров не может быть больше кол-ва точек");

    assignments.resize(points.size());
    centroids.resize(k);
    new_centroid_sums.resize(k);
    cluster_counts.resize(k);
    
    // генератор случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));
}

void KMeans::initialize_centroids() {
    vector<int> indices(points.size());
    for(int i = 0; i < points.size(); ++i) indices[i] = i;

    for (int i = 0; i < k; ++i) {
        int rand_idx = i + rand() % (points.size() - i);
        swap(indices[i], indices[rand_idx]);
    }

    for (int i = 0; i < k; ++i) {
        centroids[i] = points[indices[i]];
    }
}


void KMeans::assignment_step() {
    vector<thread> threads;
    int points_per_thread = (points.size() + max_threads - 1) / max_threads;
    
    for (int t = 0; t < max_threads; ++t) {
        int start_idx = t * points_per_thread;
        int end_idx = min(static_cast<int>(points.size()), start_idx + points_per_thread);

        if (start_idx >= end_idx) break; 

        // запускаем поток, передавая ему this и аргументы
        threads.emplace_back(&KMeans::assignment_task, this, start_idx, end_idx);
    }

    for (auto& th : threads) {
        th.join();
    }
}

void KMeans::assignment_task(int start_idx, int end_idx) {
    for (int i = start_idx; i < end_idx; ++i) {
        double min_dist = numeric_limits<double>::max();
        int closest_cluster = 0;

        for (int c = 0; c < k; ++c) {
            double dist = calculate_distance(points[i], centroids[c]);
            if (dist < min_dist) {
                min_dist = dist;
                closest_cluster = c;
            }
        }

        assignments[i] = closest_cluster;
    }
}


void KMeans::update_step() {
    for(int c = 0; c < k; ++c) {
        new_centroid_sums[c] = {0.0, 0.0};
        cluster_counts[c] = 0;
    }

    vector<thread> threads;
    int points_per_thread = (points.size() + max_threads - 1) / max_threads;

    for (int t = 0; t < max_threads; ++t) {
        int start_idx = t * points_per_thread;
        int end_idx = min(static_cast<int>(points.size()), start_idx + points_per_thread);
        
        if (start_idx >= end_idx) break;

        threads.emplace_back(&KMeans::update_task, this, start_idx, end_idx);
    }

    for (auto& th : threads) {
        th.join();
    }

    for (int c = 0; c < k; ++c) {
        if (cluster_counts[c] > 0) {
            centroids[c] = new_centroid_sums[c]; 
            centroids[c] /= cluster_counts[c]; 
        } else {

            centroids[c] = points[rand() % points.size()];
        }
    }
}

void KMeans::update_task(int start_idx, int end_idx) {
    vector<Point> local_sums(k, {0.0, 0.0});
    vector<int> local_counts(k, 0);


    for (int i = start_idx; i < end_idx; ++i) {
        int cluster_id = assignments[i];
        local_sums[cluster_id] += points[i];
        local_counts[cluster_id]++;
    }


    // блокируем мьютекс чтобы безопасно добавить локальные результаты к глобальным
    {
        lock_guard<mutex> lock(update_mutex);

        for (int c = 0; c < k; ++c) {
            new_centroid_sums[c] += local_sums[c];
            cluster_counts[c] += local_counts[c];
        }
    }
}


void KMeans::run(int max_iterations) {
    initialize_centroids();

    for (int i = 0; i < max_iterations; ++i) {
        vector<Point> old_centroids = centroids;

        assignment_step();
        update_step();

        double change = 0.0;
        for (int c = 0; c < k; ++c) {
            change += calculate_distance(old_centroids[c], centroids[c]);
        }
        
        cout << "Итерация " << i << ", Изменение: " << change << endl;
        
        if (change < 1e-5) {
            cout << "Изменили после " << i << " итераций." << endl;
            break;
        }
    }
}

const vector<Point>& KMeans::getCentroids() const {
    return centroids;
}

const vector<int>& KMeans::getAssignments() const {
    return assignments;
}
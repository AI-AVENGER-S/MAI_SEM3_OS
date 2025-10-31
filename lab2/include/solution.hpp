#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <iostream>

using namespace std;
struct Point {
    double x = 0.0;
    double y = 0.0;

    Point& operator+=(const Point& other);
    Point& operator/=(int count);
};

double calculate_distance(const Point& p1, const Point& p2);

class KMeans {
public:
    KMeans(int k, int max_threads, const vector<Point>& data);

    void run(int max_iterations = 100);

    const vector<Point>& getCentroids() const;
    const vector<int>& getAssignments() const;

private:
    int k;                     
    int max_threads;           
    vector<Point> points;
    vector<Point> centroids;
    vector<int> assignments;
    
    vector<Point> new_centroid_sums; 
    vector<int> cluster_counts;    

    mutex update_mutex;

    
    void initialize_centroids();

    // шаг присвоения - параллельный
    void assignment_step();
    // функция, выполняемая в потоке для шага присвоения
    void assignment_task(int start_idx, int end_idx);

    void update_step();

    void update_task(int start_idx, int end_idx);
};
#include <iostream>
#include <vector>
#include <cstdlib>      
#include <string>       
#include <chrono>       
#include <locale>       
#include "include/solution.hpp" 

using namespace std;

vector<Point> generate_test_data(int num_points) {
    vector<Point> data;
    data.reserve(num_points);
    
    int p_per_cluster = num_points / 3;

    auto gen_cluster = [&](int n, double cx, double cy, double spread) {
        for(int i = 0; i < n; ++i) {
            data.push_back({
                cx + (static_cast<double>(rand()) / RAND_MAX - 0.5) * spread,
                cy + (static_cast<double>(rand()) / RAND_MAX - 0.5) * spread
            });
        }
    };
    
    gen_cluster(p_per_cluster, 10, 10, 5);
    gen_cluster(p_per_cluster, -10, -10, 5);
    gen_cluster(num_points - 2 * p_per_cluster, 0, 0, 5);
    
    return data;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 3) {
        cerr << "Использование: " << argv[0] << " <макс_потоков> <кол-во_кластеров> [кол-во_точек]" << endl;
        return 1;
    }

    int max_threads = 2;
    int k = 3;
    int num_points = 1000000;

    try {
        max_threads = stoi(argv[1]);
        k = stoi(argv[2]);
        if (argc > 3) {
            num_points = stoi(argv[3]);
        }
    } catch (const exception& e) {
        cerr << "Ошибка разбора аргументов: " << e.what() << endl;
        return 1;
    }
    
    cout << "--- Лабораторная K-Means ---" << endl;
    cout << "Конфигурация:" << endl;
    cout << "  Макс. потоков:       " << max_threads << endl;
    cout << "  Кол-во кластеров (K): " << k << endl;
    cout << "  Кол-во точек:        " << num_points << endl;
    cout << "---------------------" << endl;


    cout << "Генерация " << num_points << " точек данных..." << endl;
    vector<Point> data = generate_test_data(num_points);
    cout << "Данные сгенерированы." << endl;
    cout << "Начинаем кластеризацию (может занять время)..." << endl;

    try {
        KMeans kmeans(k, max_threads, data);

        // замеряем время выполнения
        auto start_time = chrono::high_resolution_clock::now();
        
        kmeans.run(50); // запускаем максимум 50 итераций

        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end_time - start_time;

        cout << "---------------------" << endl;
        cout << "Кластеризация завершена за " << duration.count() << " секунд." << endl;
        
        cout << "\nИтоговые центроиды:" << endl;
        int c_id = 0;
        for (const auto& centroid : kmeans.getCentroids()) {
            cout << "  Кластер " << c_id++ << ": (" << centroid.x << ", " << centroid.y << ")" << endl;
        }
    
    } catch (const exception& e) {
        cerr << "Произошла ошибка во время выполнения: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
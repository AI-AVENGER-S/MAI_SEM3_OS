#include "../include/DAG.h"
#include "../include/Executor.h"
#include <iostream>
#include <iomanip>

using namespace std;

void print_dag_info(const DAG& dag) {
    cout << "\n" << string(50, '=') << "\n";
    cout << "        ИНФОРМАЦИЯ О ЗАДАЧАХ (DAG)\n";
    cout << string(50, '=') << "\n";
    cout << "лимит параллельных потоков: " << dag.max_concurrent << "\n\n";
    
    cout << left << setw(20) << "ID Джобы" 
              << "зависит от (родители)" << "\n";
    cout << string(50, '-') << "\n";

    for (auto const& [id, job] : dag.jobs) {
        string deps = "";
        for (const auto& d : job.dependencies) deps += d + " ";
        if (deps.empty()) deps = "(начальная джоба)";

        cout << left << setw(20) << id 
                  << deps << "\n";
    }
    cout << string(50, '=') << "\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "ошибка: не указан файл конфигурации.\n";
        cerr << "использование: ./dag_scheduler config.yaml\n";
        return 1;
    }

    string config_path = argv[1];

    try {
        cout << "[1/3] Загрузка файла: " << config_path << "..." << endl;
        DAG dag;
        dag.load(config_path);

        print_dag_info(dag);

        cout << "[2] проверка графа на корректность..." << endl;
        dag.validate();
        cout << ">>> проверка пройдена успешно. граф готов к запуску.\n\n";

        cout << "[3] запуск выполнения..." << endl;
        cout << "--------------------------------------------------\n";
        
        Executor executor(dag);
        executor.run();
        
        cout << "--------------------------------------------------\n";
        cout << "Все задачи выполнены." << endl;

    } catch (const YAML::BadFile& e) {
        cerr << "\n ошибка \n";
        return 1;
    } catch (const exception& e) {
        cerr << "\n ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

using namespace std;

const size_t MEMORY_SIZE = 4096;

int main() {
    string filename;
    cout << "Введите название файла для mmap: ";
    if (!(cin >> filename)) return 0;
    
    string dummy; 
    getline(cin, dummy);

    int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        return 1;
    }

    if (ftruncate(fd, MEMORY_SIZE) == -1) {
        perror("Ошибка ftruncate");
        close(fd);
        return 1;
    }

    char* ptr = (char*)mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Ошибка mmap");
        close(fd);
        return 1;
    }

    string all_input;
    string line;
    cout << "Введите строки (пустая строка или Ctrl+D для завершения ввода):" << endl;
    
    while (getline(cin, line) && !line.empty()) {
        all_input += line + "\n";
    }

    if (all_input.size() >= MEMORY_SIZE) {
        cerr << "Ошибка: Введено слишком много данных для буфера!" << endl;
        munmap(ptr, MEMORY_SIZE);
        close(fd);
        return 1;
    }

    memcpy(ptr, all_input.c_str(), all_input.size() + 1); // +1 для \0

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        munmap(ptr, MEMORY_SIZE);
        close(fd);
        return 1;
    }

    if (pid == 0) {
        munmap(ptr, MEMORY_SIZE);
        close(fd);

        execl("./child", "child", filename.c_str(), nullptr);
        
        perror("execl failed");
        exit(1);

    } else {
        // ждем пока ребенок отработает
        waitpid(pid, nullptr, 0);
        
        if (ptr[0] != '\0') {
            cout << "\n[Parent] Получены ошибки от дочернего процесса:\n";
            cout << ptr << endl;
        } else {
            cout << "\n[Parent] Ошибок не обнаружено." << endl;
        }

        munmap(ptr, MEMORY_SIZE);
        close(fd);
        
        cout << "Программа завершена." << endl;
    }

    return 0;
}
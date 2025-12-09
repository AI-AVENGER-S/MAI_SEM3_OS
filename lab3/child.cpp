#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cctype>

using namespace std;

const size_t MEMORY_SIZE = 4096;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "[Child] Ошибка: Не передано имя файла" << endl;
        return 1;
    }
    const char* filename = argv[1];

    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("[Child] Ошибка открытия файла");
        return 1;
    }

    char* ptr = (char*)mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Child] Ошибка mmap");
        close(fd);
        return 1;
    }

    string input_data(ptr);
    stringstream ss(input_data);
    string line;
    string error_log;

    cout << "=== Вывод дочернего процесса (валидные строки) ===" << endl;
    
    while (getline(ss, line)) {
        if (line.empty()) continue;

        if (isupper((unsigned char)line[0])) {
            cout << line << endl;
        } else {
            error_log += "Ошибка: строка \"" + line + "\" не начинается с заглавной буквы\n";
        }
    }
    cout << "==================================================" << endl;

    memset(ptr, 0, MEMORY_SIZE);

    if (!error_log.empty()) {
        if (error_log.size() >= MEMORY_SIZE) {
            string msg = "Error: Too many errors to fit in buffer\n";
            memcpy(ptr, msg.c_str(), msg.size() + 1);
        } else {
            memcpy(ptr, error_log.c_str(), error_log.size() + 1);
        }
    }

    munmap(ptr, MEMORY_SIZE);
    close(fd);

    return 0;
}
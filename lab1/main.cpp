#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 

using namespace std;

int main() {
    string filename;
    cout << "Enter filename: ";
    getline(std::cin, filename);

    // pipe1: parent -> child (для передачи строк)
    // pipe2: child -> parent (для передачи ошибок)
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // --- Дочерний процесс ---

        // 1. Открываем файл для записи. Флаги:
        //    O_WRONLY - только на запись
        //    O_CREAT  - создать, если не существует
        //    O_TRUNC  - очистить, если существует
        //    0644     - права доступа
        int file_fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd == -1) {
            perror("Failed to open file in child");
            _exit(1);
        }

        // Закрываем неиспользуемые концы каналов
        close(pipe1[1]); // Не пишет в pipe1
        close(pipe2[0]); // Не читает из pipe2

        // 2. Перенаправляем потоки
        dup2(pipe1[0], STDIN_FILENO);  // Стандартный ввод теперь читает из pipe1
        dup2(file_fd, STDOUT_FILENO);  // Стандартный вывод теперь пишет в файл
        dup2(pipe2[1], STDERR_FILENO);  // Стандартный вывод ошибок пишет в pipe2

        // Закрываем оригинальные файловые дескрипторы после дублирования
        close(pipe1[0]);
        close(pipe2[1]);
        close(file_fd);

        execl("./child", "child", nullptr);
        
        perror("execl failed");
        _exit(1);

    } else {
        // --- Родительский процесс ---

        // Закрываем неиспользуемые концы каналов
        close(pipe1[0]); // Не читает из pipe1
        close(pipe2[1]); // Не пишет в pipe2

        string line;
        cout << "Enter lines (or 'exit' to finish):\n";
        while (getline(std::cin, line) && line != "exit") {
            // Добавляем '\n', чтобы getline в дочернем процессе сработал
            line += '\n';
            // Отправляем строку дочернему процессу
            write(pipe1[1], line.c_str(), line.size());
        }

        // !! Важный шаг: закрываем pipe1[1], чтобы дочерний процесс получил EOF
        // и вышел из своего цикла while(getline)
        close(pipe1[1]);

        // Ждем завершения дочернего процесса
        waitpid(pid, nullptr, 0);

        // Теперь читаем все сообщения об ошибках из pipe2
        char error_buffer[2048];
        ssize_t bytes_read = read(pipe2[0], error_buffer, sizeof(error_buffer) - 1);
        
        if (bytes_read > 0) {
            error_buffer[bytes_read] = '\0';
            cout << "\n Ошибки, полученные в дочернем канале: \n" << error_buffer;
            cout << "--------------------------------\n";
        }
        
        close(pipe2[0]);
        
        cout << "\n Программа завершена. Строки были записаны в'" << filename << "'.\n";
    }

    return 0;
}
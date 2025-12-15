#include <iostream>
#include <dlfcn.h>
#include <cstring>
#include "../include/lib_functions.hpp"

using namespace std;

void* handle = nullptr;
E_func E_ptr = nullptr;
trans_func translation_ptr = nullptr;

bool loadLib(const char* path) {
    if (handle) {
        dlclose(handle);
        handle = nullptr;
        E_ptr = nullptr;
        translation_ptr = nullptr;
    }

    handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        cerr << "Ошибка загрузки библиотеки: " << dlerror() << endl;
        return false;
    }

    E_ptr = (E_func)dlsym(handle, "E");
    translation_ptr = (trans_func)dlsym(handle, "translation");

    if (!E_ptr || !translation_ptr) {
        cerr << "Ошибка загрузки функций: " << dlerror() << endl;
        dlclose(handle);
        handle = nullptr;
        return false;
    }

    cout << "Успешно загружена библиотека: " << path << endl;
    return true;
}

int main() {
    cout << "Программа 2 (Динамическая загрузка) запущена." << endl;
    
    const char* libs[] = {"./libimpl1.so", "./libimpl2.so"};
    int current_lib = 0;
    
    if (!loadLib(libs[current_lib])) {
        cerr << "Не удалось загрузить начальную библиотеку!" << endl;
        return 1;
    }
    
    cout << "Текущая библиотека: " << current_lib + 1 << endl;
    cout << "Команды:" << endl;
    cout << "  0 - переключить реализацию" << endl;
    cout << "  1 x - вычислить E(x)" << endl;
    cout << "  2 x - перевести число x" << endl;
    cout << "  3 - показать текущую библиотеку" << endl;
    cout << "  4 - выход" << endl;

    int command;
    while (true) {
        cout << "> ";
        if (!(cin >> command)) {
            break;
        }
        
        if (command == 0) {
            current_lib = (current_lib + 1) % 2;
            if (loadLib(libs[current_lib])) {
                cout << "Переключено на библиотеку " << (current_lib + 1) << endl;
            }
        } else if (command == 1) {
            int x;
            if (cin >> x) {
                if (E_ptr) {
                    float result = E_ptr(x);
                    cout << "E(" << x << ") = " << result;
                    
                    if (current_lib == 0) {
                        cout << " (библиотека 1)" << endl;
                    } else {
                        cout << " (библиотека 2)" << endl;
                    }
                } else {
                    cout << "Функция E не загружена!" << endl;
                }
            }
        } else if (command == 2) {
            long x;
            if (cin >> x) {
                if (translation_ptr) {
                    char* result = translation_ptr(x);
                    cout << "translation(" << x << ") = " << result;
                    
                    cout << " (библиотека " << (current_lib + 1) << ")" << endl;
                    
                    delete[] result;
                } else {
                    cout << "Функция translation не загружена!" << endl;
                }
            }
        } else if (command == 3) {
            cout << "Текущая библиотека: " << (current_lib + 1) << endl;
            cout << "Путь: " << libs[current_lib] << endl;
        } else if (command == 4) {
            break;
        } else {
            cout << "Неизвестная команда! Допустимые: 0,1,2,3,4" << endl;
            cin.clear();
        }
    }

    if (handle) {
        dlclose(handle);
    }
    
    cout << "Программа завершена." << endl;
    return 0;
}
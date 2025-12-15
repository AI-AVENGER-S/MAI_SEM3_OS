#include <iostream>
#include "../include/lib_functions.hpp"

using namespace std;

int main() {
    int command;
    cout << "Программа 1 (Статическая линковка) запущена." << endl;
    cout << "Введите команду (1 x - расчет E, 2 x - перевод числа):" << endl;

    while (cin >> command) {
        if (command == 1) {
            int x;
            cin >> x;
            cout << "Результат E(" << x << "): " << E(x) << endl;
        } else if (command == 2) {
            long x;
            cin >> x;
            char* res = translation(x);
            cout << "Результат перевода (" << x << "): " << res << endl;
            delete[] res;
        }
    }
    return 0;
}
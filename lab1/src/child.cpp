#include <iostream>
#include <string>
#include "solution.hpp"
#include <cctype>

using namespace std;


int main() {
    string line;
    while (getline(cin, line)) {
        if (startsWithUppercase(line)) {
            cout << line << endl;
        } else {
            cerr << "Ошибка: '" << line << "' - не с заглавной буквы (либо пустая))" << endl;
        }
    }
    return 0;
}
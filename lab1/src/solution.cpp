#include "solution.hpp"
#include <cctype>

using namespace std;

bool startsWithUppercase(const string& str) {
    if (str.empty()) {
        return false;
    }
    return isupper(static_cast<unsigned char>(str[0]));
}
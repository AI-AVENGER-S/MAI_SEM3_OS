#include "../include/lib_functions.hpp"
#include <cmath>
#include <string>
#include <cstring>

extern "C" {
    float E(int x) {
        if (x <= 0) return 0.0f;
        return std::pow(1.0f + 1.0f / x, static_cast<float>(x));
    }

    char* translation(long x) {
        if (x == 0) return strdup("0");
        std::string res = "";
        unsigned long val = static_cast<unsigned long>(x);
        while (val > 0) {
            res = std::to_string(val % 2) + res;
            val /= 2;
        }
        char* cstr = new char[res.length() + 1];
        std::strcpy(cstr, res.c_str());
        return cstr;
    }
}
#include "../include/lib_functions.hpp"
#include <string>
#include <cstring>

extern "C" {
    float E(int x) {
        float e = 0.0f;
        float fact = 1.0f;
        for (int n = 0; n <= x; ++n) {
            if (n > 0) fact *= n;
            e += 1.0f / fact;
        }
        return e;
    }

    char* translation(long x) {
        if (x == 0) return strdup("0");
        std::string res = "";
        unsigned long val = static_cast<unsigned long>(x);
        while (val > 0) {
            res = std::to_string(val % 3) + res;
            val /= 3;
        }
        char* cstr = new char[res.length() + 1];
        std::strcpy(cstr, res.c_str());
        return cstr;
    }
}
#ifndef LIB_FUNCTIONS_HPP
#define LIB_FUNCTIONS_HPP

typedef float (*E_func)(int);
typedef char* (*trans_func)(long);


extern "C" {
    float E(int x);
    char* translation(long x);
}

#endif
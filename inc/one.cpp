/*  This file is meant as a test to see if imports from CPP are working.
    It also checks if the C++ compiler is really compiling the file as opposed to the C compiler. */



#ifdef __cplusplus
extern "C" {
    #include <cstdarg>
    #include "one.h"
    int one(){
        return 1;
    }
}
#endif

#ifndef __cplusplus
#include "one.h"
int one(){
    return 2;
}
#endif
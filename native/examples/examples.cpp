// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "examples.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <iomanip>

void *__dso_handle = (void *) &__dso_handle;

class Timer {
public:
    Timer() {
        print_timestamp("Program started at: ");
    }
    
    ~Timer() {
        print_timestamp("Program ended at: ");
    }

private:
    static void print_timestamp(const char* prefix) {
        unsigned int aux;
        auto cycles = __rdtscp(&aux);
        std::cout << prefix 
                 << cycles
                 << std::endl;
    }
};

// Global timer instance
static Timer global_timer;

using namespace std;
using namespace seal;

int main()
{
    viand2023("tiny");
    viand2023("small");
    viand2023("medium");
    return 0;
}

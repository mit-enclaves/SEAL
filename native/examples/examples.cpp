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
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        
        // Get seconds and nanoseconds separately
        auto secs = duration_cast<std::chrono::seconds>(duration);
        auto nsecs = duration_cast<std::chrono::nanoseconds>(duration - secs);
        
        // Print with exact same format as date +%s.%N
        std::cout << prefix 
                 << secs.count() << "."
                 << std::setfill('0') << std::setw(9) << nsecs.count()
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

// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "examples.h"
#include <stdio.h>
#include <iostream>

void *__dso_handle = (void *) &__dso_handle;

using namespace std;
using namespace seal;

int main()
{
    printf("Hello World!\n");
    cout << "Microsoft SEAL version: " << endl;
 
    /*
    Print how much memory we have allocated from the current memory pool.
    By default the memory pool will be a static global pool and the
    MemoryManager class can be used to change it. Most users should have
    little or no reason to touch the memory allocation system.
    */
    size_t megabytes = MemoryManager::GetPool().alloc_byte_count() >> 20;
    cout << "[" << setw(7) << right << megabytes << " MB] "
         << "Total allocation from the memory pool" << endl;
    example_bfv_basics();
    
    return 0;
}

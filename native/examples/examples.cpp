// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "examples.h"
#include <stdio.h>
#include <iostream>

//void *__dso_handle = (void *) &__dso_handle;

using namespace std;
using namespace seal;

int main()
{
    viand2023("tiny");
    viand2023("small");
    viand2023("medium");
    return 0;
}

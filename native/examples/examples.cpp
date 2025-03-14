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

// Transcript vmcall examples

#define FAILURE (-1)
typedef unsigned long long usize;

#define ARGOS_GET_SIGNING_KEY 33
#define ARGOS_APPEND 36
#define ARGOS_GET_SIGNED_TRANSCRIPT 37

#define HASH_LEN 32
#define SIG_LEN 64
#define KEY_LEN 64

typedef struct vmcall_frame_t {
	// Vmcall id.
	usize vmcall;

	// Arguments.
	usize arg_1;
	usize arg_2;
	usize arg_3;
	usize arg_4;
	usize arg_5;
	usize arg_6;

	// Results.
	usize value_1;
	usize value_2;
	usize value_3;
	usize value_4;
	usize value_5;
	usize value_6;
} vmcall_frame_t;

static int vmcall(void);
static int tyche_call(vmcall_frame_t *frame);
static int append(uint8_t * buff, size_t len, bool should_hash);
static int get_transcript(uint8_t * transcript, uint8_t * signature);
static int get_signing_key(uint8_t * signing_key);

int main()
{
    viand2023("tiny");
    viand2023("small");
    viand2023("medium");

    // Example of appending to transcript,
    // obtaining final transcript + sig + signing key

    // ptr into confidential memory
    uint8_t * buff = (uint8_t *)0xa00000;

    // Append a hash of 32 zero bytes to transcript
    memset(buff, 0, 32);
    append(buff, 32, 1);

    // Get the final transcript + signature
    // Final transcript = H(measurement (see logs) || H(bytes(32)))
    uint8_t * transcript = buff;
    uint8_t * signature = buff + HASH_LEN;
    get_transcript(transcript, signature);
    printf("transcript: ");
    for (int i = 0; i < HASH_LEN; i++) {
        printf("%02X", transcript[i]);
    }
    printf("\nsignature: ");
    for (int i = 0; i < SIG_LEN; i++) {
        printf("%02X", signature[i]);
    }
    printf("\n");

    // Print the signing key used.
    uint8_t * signing_key = buff;
    get_signing_key(signing_key);
    printf("signing key: x=");
    for (int i = 0; i < KEY_LEN/2; i++) {
        printf("%02X", signing_key[i]);
    }
    printf(", y=");
    for (int i = KEY_LEN/2; i < KEY_LEN; i++) {
        printf("%02X", signing_key[i]);
    }
    printf("\n");

    return 0;
}

/// Simple generic vmcall implementation.
int tyche_call(vmcall_frame_t *frame)
{
  usize result = FAILURE;
  asm volatile(
    // Setting arguments.
    "movq %7, %%rax\n\t"
    "movq %8, %%rdi\n\t"
    "movq %9, %%rsi\n\n"
    "movq %10, %%rdx\n\t"
    "movq %11, %%rcx\n\t"
    "movq %12, %%r8\n\t"
    "movq %13, %%r9\n\t"
    "vmcall\n\t"
    // Receiving results.
    "movq %%rax, %0\n\t"
    "movq %%rdi, %1\n\t"
    "movq %%rsi, %2\n\t"
    "movq %%rdx, %3\n\t"
    "movq %%rcx, %4\n\t"
    "movq %%r8,  %5\n\t"
    "movq %%r9,  %6\n\t"
    : "=rm" (result), "=rm" (frame->value_1), "=rm" (frame->value_2), "=rm" (frame->value_3), "=rm" (frame->value_4), "=rm" (frame->value_5), "=rm" (frame->value_6)
    : "rm" (frame->vmcall), "rm" (frame->arg_1), "rm" (frame->arg_2), "rm" (frame->arg_3), "rm" (frame->arg_4), "rm" (frame->arg_5), "rm" (frame->arg_6)
    : "rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "memory");
  return (int)result;
}

int append(uint8_t * buff, size_t len, bool should_hash) {
    vmcall_frame_t frame = {0};
    frame.vmcall = ARGOS_APPEND;
    frame.arg_1 = (usize) buff;
    frame.arg_2 = should_hash ? len : HASH_LEN;
    frame.arg_3 = should_hash;
    frame.arg_4 = 1; // buffs are specified in GVA

    return tyche_call(&frame);
}

int get_transcript(uint8_t * transcript, uint8_t * signature) {
    vmcall_frame_t frame = {0};
    frame.vmcall = ARGOS_GET_SIGNED_TRANSCRIPT;
    frame.arg_1 = (usize) transcript;
    frame.arg_2 = HASH_LEN;
    frame.arg_3 = (usize) signature;
    frame.arg_4 = SIG_LEN;
    frame.arg_5 = 1; // buffs are specified in GVA

    return tyche_call(&frame);
}

int get_signing_key(uint8_t * signing_key) {
    vmcall_frame_t frame = {0};
    frame.vmcall = ARGOS_GET_SIGNING_KEY;
    frame.arg_1 = (usize) signing_key;
    frame.arg_2 = KEY_LEN;
    frame.arg_3 = 1; // buffs are specified in GVA

    return tyche_call(&frame);
}

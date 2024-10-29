# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.13)

project(SEALExamples VERSION 4.1.1 LANGUAGES CXX)

get_filename_component(CUR_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# If not called from root CMakeLists.txt
if(NOT DEFINED SEAL_BUILD_EXAMPLES)
    set(SEAL_BUILD_EXAMPLES ON)

    # Import Microsoft SEAL
    find_package(SEAL 4.1.1 EXACT REQUIRED)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(SEAL_BUILD_EXAMPLES)
    add_executable(sealexamples)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,${CUR_DIR}/eh_frame_symbols.ld")

    target_sources(sealexamples
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/examples.cpp
            ${CMAKE_CURRENT_LIST_DIR}/viand2023.cpp
    )

    if(TARGET SEAL::seal)
        target_link_libraries(sealexamples PRIVATE SEAL::seal c++ c++abi clang_rt.builtins-x86_64 m pthread dl util rt c)
    elseif(TARGET SEAL::seal_shared)
        target_link_libraries(sealexamples PRIVATE SEAL::seal_shared)
    else()
        message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
    endif()

endif()

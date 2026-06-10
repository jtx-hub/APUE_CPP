cmake_minimum_required(VERSION 3.16)
project(memory_crash_simulator)

set(CMAKE_CXX_STANDARD 11)

add_executable(memory_crash_simulator 
    MemoryCrashSimulator.cpp
)
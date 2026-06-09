cmake_minimum_required(VERSION 3.16)
project(file_descriptor)

set(CMAKE_CXX_STANDARD 11)

add_executable(file_descriptor 
    main.cpp
    FileDescriptor.cpp
)
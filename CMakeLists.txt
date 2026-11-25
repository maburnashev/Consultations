cmake_minimum_required(VERSION 3.10)

project(consultations)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MINIMUM_REQUIRED_VERSION ON)

add_executable(consultations 
    main.cpp
    src/banking.cpp    
)
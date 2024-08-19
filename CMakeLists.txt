cmake_minimum_required(VERSION 3.5.1)
cmake_policy(SET CMP0071 NEW)
project(IDE)

## Set compiler to use c++ 11 features
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)

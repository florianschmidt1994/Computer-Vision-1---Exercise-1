cmake_minimum_required(VERSION 3.1.1)
project(Exercise_1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp)

find_package( OpenCV REQUIRED )

add_executable(Exercise_1 ${SOURCE_FILES})

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Exercise_1 ${OpenCV_LIBS})
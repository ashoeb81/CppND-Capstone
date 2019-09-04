cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(HelloWorld)

find_package(OpenCV 4.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

find_package(Boost COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)

file(GLOB project_SRC src/*.cpp src/*.h)
add_executable(HelloWorld ${project_SRC})

target_link_libraries (HelloWorld ${OpenCV_LIBRARIES})
target_link_libraries(HelloWorld ${Boost_LIBRARIES})
target_link_libraries(HelloWorld Threads::Threads)

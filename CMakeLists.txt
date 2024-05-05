cmake_minimum_required(VERSION 3.27)
project(Chat_client_project)

set(CMAKE_CXX_STANDARD 17)


add_executable(Chat_client_project main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Chat_client_project Threads::Threads)

target_link_libraries(Chat_client_project wsock32 ws2_32)




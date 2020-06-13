cmake_minimum_required(VERSION 3.10)

project(Nanozombie)

include_directories(${PROJECT_SOURCE_DIR})


# Debug modes
add_compile_definitions(DEBUG_USE_COLOR)
add_compile_definitions(DEBUG_SHOW_SUBMAR_QUEUE)
# add_compile_definitions(DEBUG_PRINT_ONLY_IMPORTANT_STUFF)
# add_compile_definitions(DEBUG_SHOW_PID)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -g -ggdb")
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(nanozombie.out main.cpp communication_loop.cpp condition_var.cpp debug.cpp main_loop.cpp packet.cpp queues_submarine.cpp system_info.cpp tourist.cpp)

find_package(Threads REQUIRED)
target_link_libraries(nanozombie.out Threads::Threads)

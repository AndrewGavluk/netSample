cmake_minimum_required( VERSION 3.5)
project (sample1)
add_executable(sample1 main.cpp)
target_link_libraries(sample1 pthread)
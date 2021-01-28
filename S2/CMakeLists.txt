cmake_minimum_required( VERSION 3.5)
project (sample2 C CXX)

add_executable(sample2 simpleClient.cpp)
#set_target_properties(sample2 PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQIRED ON)
#target_link_libraries(sample2 PRIVATE pthread)
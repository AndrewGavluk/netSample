cmake_minimum_required( VERSION 3.5)
project (sample2 C CXX)

add_executable(sample2_client simpleClient.cpp)
set_target_properties(sample2_client PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQIRED ON)
target_link_libraries(sample2_client PRIVATE pthread)

add_executable(sample2_server simpleServer.cpp)
set_target_properties(sample2_server PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQIRED ON)
target_link_libraries(sample2_server PRIVATE pthread)
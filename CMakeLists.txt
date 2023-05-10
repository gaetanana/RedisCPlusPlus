cmake_minimum_required(VERSION 3.17)
project(RedisCPlusPlus)

set(CMAKE_CXX_STANDARD 14)

include_directories("C:/Users/g.gonfiantini/Desktop/RedisCPlusPlus/DriverHiredis/hiredis/build/install/include")
link_directories("C:/Users/g.gonfiantini/Desktop/RedisCPlusPlus/DriverHiredis/hiredis/build/install/lib")

add_executable(RedisCPlusPlus main.cpp)

target_link_libraries(RedisCPlusPlus hiredisd)
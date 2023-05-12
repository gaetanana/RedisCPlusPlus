cmake_minimum_required(VERSION 3.17)
project(RedisCPlusPlus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)

include_directories("DriverHiredis/hiredis/build/install/include")
include_directories("DriverPugiXML/pugixml/build/install/include")
include_directories("DriverJSON/jsoncpp/build/install/include")

link_directories("DriverHiredis/hiredis/build/install/lib")
link_directories("DriverPugiXML/pugixml/build/install/lib")
link_directories("DriverJSON/jsoncpp/build/install/lib")

add_executable(RedisCPlusPlus main.cpp CREATE/CREATE.cpp CREATE/CREATE.h ConnectionRedis/connection.cpp ConnectionRedis/connection.h DELETE/DELETE.cpp DELETE/DELETE.h READ/READ.cpp READ/READ.h)

target_link_libraries(RedisCPlusPlus hiredisd)
target_link_libraries(RedisCPlusPlus pugixml)
target_link_libraries(RedisCPlusPlus jsoncpp)

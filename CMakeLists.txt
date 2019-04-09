cmake_minimum_required(VERSION 3.10)

PROJECT(EasyNet)

set(CMAKE_CXX_COMPILER "g++") 
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall") 
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG") 
#set(PROJECT_SOURCE_DIR "./")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build) 
Set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build) 
add_definitions(-std=c++11)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})


#message(STATUS "Project source dir="${PROJECT_SOURCE_DIR})

aux_source_directory(${PROJECT_SOURCE_DIR}/EasyNet/src/base  EasyNet_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/EasyNet/src/net EasyNet_SRCS)



ADD_LIBRARY(EasyNet SHARED ${EasyNet_SRCS})
target_link_libraries(EasyNet pthread rt protobuf)

link_directories(${PROJECT_BINARY_DIR}/build)
ADD_EXECUTABLE(ChatServer ${PROJECT_SOURCE_DIR}/EasyNet/src/ut/ChatServer.cpp)
ADD_EXECUTABLE(ChatClient ${PROJECT_SOURCE_DIR}/EasyNet/src/ut/ChatClient.cpp)
target_link_libraries(ChatServer pthread rt protobuf EasyNet)
target_link_libraries(ChatClient pthread rt protobuf EasyNet)






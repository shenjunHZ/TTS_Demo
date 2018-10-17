#COpyright (c) 2018 junshen

cmake_minimum_required(VERSION 3.4)
project(kdDemo)
set(EXECUTABLE_NAME "kdDemo")

if ( CMAKE_C_COMPILER MATCHES "gcc$")
    include(./make/gnu/sanitizers.cmake)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(Boost_USE_STATIC_LIBS TRUE)
    set(Boost_USE_STATIC_RUNTIME TRUE)
endif()

LINK_DIRECTORIES(
        ${CMAKE_CURRENT_LIST_DIR}/tts/libs/x64
        /home/shenjun/tools/boost_1_65_0/gcc/lib
        /home/shenjun/tools/libzmq/cmake-build/lib
        /usr/lib
    )

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_LIST_DIR}/tts/include
        ${CMAKE_CURRENT_LIST_DIR}/src
        /home/shenjun/tools/boost_1_65_0/boost
        /home/shenjun/tools/libzmq/include
        /home/shenjun/tools/spdlog-1.x/include
    )

set(SRC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

MESSAGE(STATUS "kdDemo")

add_compile_options(
        -g
        -Wall
        -Werror
        #-fexec-charset=GBK
        -lasound
)

SET(COMMON_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -lpthread -std=c++14 \
    -lboost_program_options -lboost_system -lboost_thread")

#find_package(PkgConfig)
#find_package(Boost COMPONENTS REQUIRED system filesystem)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#endif()
set(Boost_INCLUDE_DIRS "/home/shenjun/tools/boost_1_65_0/boost")
set(Boost_LIBRARIES "/home/shenjun/tools/boost_1_65_0/gcc/lib")

set(LIBTTS libmsc.so)
set(LIBZMQ libzmq.so)

MESSAGE(STATUS "kdDemo")

ADD_SUBDIRECTORY(src)

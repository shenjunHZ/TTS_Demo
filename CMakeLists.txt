#Copyright (c) 2018 junshen

cmake_minimum_required(VERSION 3.4)
project(kdDemo)
set(EXECUTABLE_NAME "kdDemo")

if ( ${CMAKE_C_COMPILER} MATCHES "/home/shenjun/tools/gcc-linaro-5.4.1-2017.01-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
    set(ARM_LINUX TRUE)
    add_definitions(-DRUN_IN_ARM)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(Boost_USE_STATIC_LIBS TRUE)
    set(Boost_USE_STATIC_RUNTIME TRUE)
endif()

#find_package(PkgConfig)
#set(BOOST_ROOT /home/shenjun/tools/boost_1_65_0/gcc)
#find_package(Boost 1.65.0 REQUIRED COMPONENTS system filesystem)

#find_package(Zmq REQUIRED)
if(${ARM_LINUX})
    LINK_DIRECTORIES(
            ${CMAKE_CURRENT_LIST_DIR}/linux_aisound_exp1218_5be7bc2b/libs/arm32
            /home/shenjun/tools/alsa-lib/arm-alsa/lib
            /home/shenjun/tools/boost_1_65_0/arm/lib
    )
else()
    LINK_DIRECTORIES(
            ${CMAKE_CURRENT_LIST_DIR}/linux_aisound_exp1218_5be7bc2b/libs/x64
            /hmoe/shenjun/tools/alsa-lib-1.1.6/gcc-alsa/lib
            /home/shenjun/tools/boost_1_65_0/gcc/lib
    )
endif()

LINK_DIRECTORIES(
        /home/shenjun/tools/libzmq/cmake-build/lib
    )

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_LIST_DIR}/linux_aisound_exp1218_5be7bc2b/include
        ${CMAKE_CURRENT_LIST_DIR}/src
        /home/shenjun/tools/boost_1_65_0/arm/include
        /home/shenjun/tools/libzmq/include
        /home/shenjun/tools/spdlog-1.x/include
        /home/shenjun/tools/alsa-lib/arm-alsa/include
    )

set(SRC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

MESSAGE(STATUS "kdDemo")

add_compile_options(
        -g
        -Wall
        -Werror
        #-fexec-charset=GBK
        #-lasound
)

SET(COMMON_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -lpthread -std=c++14")
    #-lboost_program_options -lboost_system -lboost_thread -llibasound

#set(Boost_INCLUDE_DIRS "/home/shenjun/tools/boost_1_65_0/boost")
#set(Boost_LIBRARIES "/home/shenjun/tools/boost_1_65_0/gcc/lib")

set(LIBTTS libmsc.so)
set(LIBASOUND libasound.so)

MESSAGE(STATUS "kdDemo")

ADD_SUBDIRECTORY(src)

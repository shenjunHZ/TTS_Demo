#Copyright (c) 2018 junshen

set (SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/Main.cpp
        #${CMAKE_CURRENT_LIST_DIR}/applications/ZmqReceiver.cpp
        applications/AppInstance.cpp
        applications/ClientReceiver.cpp
        applications/KDTTSService.cpp
        socket/SocketSysCall.cpp
        socket/ClientSocket.cpp
        configurations/ConfigurationLoader.cpp
        player/AlsaPlayer.cpp
        player/CodeConverter.cpp
        player/SystemPlayer.cpp)

set(INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}/../include
        )

include_directories(${INCLUDES})

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
            ${LIBTTS}
            ${LIBASOUND}
            #libasound
            #${Boost_LIBRARIES}
            boost_program_options
            boost_thread
            boost_system
            logger
            timer
        )

ADD_SUBDIRECTORY(logger)
ADD_SUBDIRECTORY(timer)
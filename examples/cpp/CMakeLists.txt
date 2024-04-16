project(SwarmGameCppExample)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZMQ_STATIC")

add_executable(${PROJECT_NAME}
    main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
    Controller.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${JSON_LIB_DIR}/single_include
    ${ZeroMQ_INCLUDE_DIRS}
    ${CPPZMQ_LIB_DIR}
)

if (UNIX)
    set(OS_DEPENDENT_LIBRARIES
    )
elseif (WIN32)
    set(OS_DEPENDENT_LIBRARIES
        "wsock32.lib"
        "ws2_32.lib"
        "Iphlpapi.lib"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ZeroMQ_LIBRARIES}
    ${OS_DEPENDENT_LIBRARIES}
)
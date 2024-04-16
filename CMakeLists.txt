project(SwarmGame)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZMQ_STATIC")

# Configure libraries
set(PROJECT_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
# cppack
set(CPPACK_LIB_DIR ${PROJECT_LIB_DIR}/cppack)
# json
set(JSON_LIB_DIR ${PROJECT_LIB_DIR}/json-3.11.3)
# raylib
add_subdirectory(${PROJECT_LIB_DIR}/raylib-5.0)
# raygui
set(RAYGUI_LIB_DIR ${PROJECT_LIB_DIR}/raygui-4.0)
# zeromq
set(ZEROMQ_LIB_DIR ${PROJECT_LIB_DIR}/libzmq-4.3.5)
find_library(ZEROMQ_LIB_RELEASE
    NAMES zmq-v143-mt-s-4_3_5 libzmq-v143-mt-s-4_3_5
    PATHS ${ZEROMQ_LIB_DIR}/lib)
if (NOT ZEROMQ_LIB_RELEASE)
    message(FATAL_ERROR "ZeroMQ Release lib not found.") 
endif()
find_library(ZEROMQ_LIB_DEBUG
    NAMES zmq-v143-mt-sgd-4_3_5 libzmq-v143-mt-sgd-4_3_5
    PATHS ${ZEROMQ_LIB_DIR}/lib)
if (NOT ZEROMQ_LIB_DEBUG)
    message(FATAL_ERROR "ZeroMQ Debug lib not found.")
endif()
set(ZeroMQ_LIBRARIES
    "$<$<NOT:$<CONFIG:DEBUG>>:${ZEROMQ_LIB_RELEASE}>"
    "$<$<CONFIG:DEBUG>:${ZEROMQ_LIB_DEBUG}>"
)
set(ZeroMQ_INCLUDE_DIRS
    ${ZEROMQ_LIB_DIR}/include
)
# cppzmq
set(CPPZMQ_LIB_DIR ${PROJECT_LIB_DIR}/cppzmq-4.10.0)

add_executable(${PROJECT_NAME}
    src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${JSON_LIB_DIR}/single_include
    ${RAYGUI_LIB_DIR}/src
    ${ZeroMQ_INCLUDE_DIRS}
    ${CPPZMQ_LIB_DIR}
)

target_sources(${PROJECT_NAME} PRIVATE
    src/common/FileUtils.cpp
    src/common/Module.cpp
    src/common/Uav.cpp
    src/sim/Model.cpp
    src/vis/CameraController.cpp
    src/vis/Visualiser.cpp
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
    raylib
    ${ZeroMQ_LIBRARIES}
    ${OS_DEPENDENT_LIBRARIES}
)

add_subdirectory(examples/cpp)
cmake_minimum_required(VERSION 3.10)

SET(SUB_PROJECT_NAME EngineCollModbus)

SET(SUB_PROJECT_DIR ${PROJECT_SOURCE_DIR}/${SUB_PROJECT_NAME})
#SET(CMAKE_VERBOSE_MAKEFILE  ON)
MESSAGE(STATUS "-------------- project is ${SUB_PROJECT_NAME} ----------------")

SET(SRC_LIST
    ./CollModbus.cpp
        )

execute_process(
        COMMAND bash "-c" "git rev-list HEAD -n 1 | cut -c 1-7"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
MESSAGE(STATUS "version is ${GIT_VERSION}")
add_definitions(-DLibVersion=\"${GIT_VERSION}\" -DLibName=\"${SUB_PROJECT_NAME}\")

IF (CMAKE_CXX_COMPILER MATCHES "aarch64-linux")
    add_definitions(-DX64)
ELSEIF (CMAKE_CXX_COMPILER MATCHES "arm-linux")
    add_definitions(-DX32)
ELSE ()
    add_definitions(-DX64)
ENDIF ()

IF (lib MATCHES "shared")
    MESSAGE(STATUS "build shared lib")
    ADD_LIBRARY(${SUB_PROJECT_NAME} SHARED ${SRC_LIST})
ELSEIF (lib MATCHES "static")
    MESSAGE(STATUS "build static lib")
    ADD_LIBRARY(${SUB_PROJECT_NAME} STATIC ${SRC_LIST})
ELSE ()
    MESSAGE(STATUS "build shared lib")
    ADD_LIBRARY(${SUB_PROJECT_NAME} SHARED ${SRC_LIST})
ENDIF ()

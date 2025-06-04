cmake_minimum_required(VERSION 3.7)
project(DataEngine)
MESSAGE(STATUS "project is " ${CMAKE_PROJECT_NAME})
SET(CMAKE_VERBOSE_MAKEFILE  ON)
#file(GLOB src_dir "./*.c")
SET(CMAKE_SYSTEM_NAME Linux)
#option(platform "platform option" arm)
IF(platform MATCHES "arm")
MESSAGE(STATUS "arm platform build")
SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
ELSEIF(platform MATCHES "x86")
MESSAGE(STATUS "x86 platform build")
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
ELSEIF(platform MATCHES "win")
MESSAGE(STATUS "win platform build")
add_definitions(-DWIN32)
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
ELSE()
MESSAGE(STATUS "aarch64 platform build")
SET(platform "aarch64")
SET(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
SET(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")
ENDIF()
include_directories("./" "${PROJECT_SOURCE_DIR}/Include/BaseInc" "${PROJECT_SOURCE_DIR}/Src" "${PROJECT_SOURCE_DIR}/Include/ThirdInc" "${PROJECT_SOURCE_DIR}/Include/UtilsInc")
link_directories("${PROJECT_SOURCE_DIR}/Lib/ThirdLib/${platform}" "${PROJECT_SOURCE_DIR}/Lib/BusinessLib/${platform}" "${PROJECT_SOURCE_DIR}/Lib/UtilsLib/${platform}" "${PROJECT_SOURCE_DIR}/Lib/BaseLib/${platform}")
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    MESSAGE(STATUS "${CMAKE_PROJECT_NAME} Build Debug Program")
ELSE()
    MESSAGE(STATUS "${CMAKE_PROJECT_NAME} Build Release Program")
ENDIF()
execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/out)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/${platform}/${CMAKE_BUILD_TYPE})
#link_libraries("dl" "pthread" "framework" "avcodec" "avformat" "avutil" "swresample" "rockchip_mpp" "opencv_core" "opencv_highgui" "opencv_imgcodecs" "opencv_imgproc" "opencv_videoio")
#SET(CMAKE_BUILD_TYPE "Debug")
IF(platform MATCHES "win")
SET(CMAKE_CXX_FLAGS_DEBUG "-g")
ELSE()
SET(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic")
ENDIF()
SET(CMAKE_CXX_FLAGS_RELEASE "-Os")

SET(LINK_COMM
    "pthread"
    "jsoncpp"
    )
IF(NOT (platform MATCHES "win"))
SET(LINK_COMM ${LINK_COMM} "dl")
ELSE()
SET(LINK_COMM ${LINK_COMM} "framework")
ENDIF()
SET(SRC_LIST
    ./DataEngine.cpp
    ./Src/Tool/Tool.cpp
    ./Src/Config/ConfigParse.cpp
    ./Src/Tool/ExpParser.cpp
    ./Src/Channel/ChannelManager.cpp
    ./Src/Channel/RS485Channel.cpp
    ./Src/Channel/TCPServerChannel.cpp
    ./Src/Channel/IChannel.cpp
    ./Src/Channel/TCPClientChannel.cpp
    ./Src/Channel/UDPChannel.cpp
    ./Src/Task/TaskCenter.cpp
    ./Src/Protocol/IProtocol.cpp
    ./Src/Protocol/ProtocolManager.cpp
    ./Src/Data/DataStorage.cpp
    ./Src/Log/LogManager.cpp
    ./Src/Log/Log.cpp
    )
#execute_process(COMMAND mkdir ./bin)
execute_process(
	COMMAND	bash "-c" "git rev-list HEAD -n 1 | cut -c 1-7"
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
MESSAGE(STATUS "version is ${GIT_VERSION}")
add_definitions(-std=c++11 -Wall -DLibVersion=\"${GIT_VERSION}\" -DLibName=\"${CMAKE_PROJECT_NAME}\")
#set(LIBRARY_OUTPUT_PATH ./bin)
#add_library(show SHARED ${SRC_LIST})

add_subdirectory(Plug)

#PROJECT(HELLO)
#SET(SRC_LIST main.c)
#MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})
#MESSAGE(STATUS "This is SOURCE dir "${HELLO_SOURCE_DIR})
IF(lib MATCHES "shared")
MESSAGE(STATUS "build shared lib")
ADD_LIBRARY(${CMAKE_PROJECT_NAME} SHARED ${SRC_LIST})
ELSEIF(lib MATCHES "static")
MESSAGE(STATUS "build static lib")
ADD_LIBRARY(${CMAKE_PROJECT_NAME} STATIC ${SRC_LIST})
ELSE()
MESSAGE(STATUS "build shared lib")
ADD_LIBRARY(${CMAKE_PROJECT_NAME} SHARED ${SRC_LIST})
ENDIF()
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LINK_COMM})

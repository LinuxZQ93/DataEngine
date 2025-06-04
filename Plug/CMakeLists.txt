cmake_minimum_required(VERSION 3.10)


include_directories("${PROJECT_SOURCE_DIR}/Include/BusinessInc")

SET(CMAKE_CXX_FLAGS_DEBUG "-fPIC  -g -rdynamic -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-fPIC -Os -rdynamic -Wall")

link_libraries("DataEngine")
add_subdirectory(CollModbus)
add_subdirectory(IEC104)

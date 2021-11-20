cmake_minimum_required(VERSION 3.12)

project(midi-cube VERSION 1.0
    DESCRIPTION "A C++ synthesizer workstation"
    LANGUAGES CXX)

file(GLOB_RECURSE FRAMEWORK_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/framework/*.cpp")
file(GLOB_RECURSE PLUGINS_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/plugins/*.cpp")
file(GLOB_RECURSE WORKSTATION_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/workstation/*.cpp")

add_executable(midi-cube-workstation ${FRAMEWORK_SOURCES} ${PLUGINS_SOURCES} ${WORKSTATION_SOURCES} "${CMAKE_SOURCE_DIR}/midicube/resources.h" "${CMAKE_SOURCE_DIR}/midicube/resources.cpp" "${CMAKE_SOURCE_DIR}/midicube-main.cpp")
add_executable(midi-cube-autosampler ${FRAMEWORK_SOURCES} ${PLUGINS_SOURCES} ${WORKSTATION_SOURCES} "${CMAKE_SOURCE_DIR}/midicube/resources.h" "${CMAKE_SOURCE_DIR}/midicube/resources.cpp" "${CMAKE_SOURCE_DIR}/autosampler-main.cpp")


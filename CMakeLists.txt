cmake_minimum_required(VERSION 3.12)

project(midi-cube VERSION 1.0
    DESCRIPTION "A C++ synthesizer workstation"
    LANGUAGES CXX)

file(GLOB_RECURSE FRAMEWORK_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/framework/*.cpp")
file(GLOB_RECURSE PLUGINS_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/plugins/*.cpp")
file(GLOB_RECURSE WORKSTATION_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/workstation/*.cpp")

add_executable(midi-cube-workstation ${FRAMEWORK_SOURCES} ${PLUGINS_SOURCES} ${WORKSTATION_SOURCES} "${CMAKE_SOURCE_DIR}/midicube/resources.h" "${CMAKE_SOURCE_DIR}/midicube/resources.cpp" "${CMAKE_SOURCE_DIR}/midicube-main.cpp")
add_executable(midi-cube-autosampler ${FRAMEWORK_SOURCES} ${PLUGINS_SOURCES} ${WORKSTATION_SOURCES} "${CMAKE_SOURCE_DIR}/midicube/resources.h" "${CMAKE_SOURCE_DIR}/midicube/resources.cpp" "${CMAKE_SOURCE_DIR}/autosampler-main.cpp")

target_link_libraries(midi-cube-workstation PUBLIC rtaudio)
target_link_libraries(midi-cube-workstation PUBLIC rtmidi)
target_link_libraries(midi-cube-workstation PUBLIC sndfile)
target_link_libraries(midi-cube-workstation PUBLIC sfml-system)
target_link_libraries(midi-cube-workstation PUBLIC sfml-window)
target_link_libraries(midi-cube-workstation PUBLIC sfml-graphics)
target_link_libraries(midi-cube-workstation PUBLIC threads)
target_link_libraries(midi-cube-workstation PUBLIC boost)

target_link_libraries(midi-cube-autosampler PUBLIC rtaudio)
target_link_libraries(midi-cube-autosampler PUBLIC rtmidi)
target_link_libraries(midi-cube-autosampler PUBLIC sndfile)
target_link_libraries(midi-cube-autosampler PUBLIC sfml-system)
target_link_libraries(midi-cube-autosampler PUBLIC sfml-window)
target_link_libraries(midi-cube-autosampler PUBLIC sfml-graphics)
target_link_libraries(midi-cube-autosampler PUBLIC threads)
target_link_libraries(midi-cube-autosampler PUBLIC boost)

target_compile_features(midi-cube-workstation PUBLIC cxx_std_17)
target_compile_features(midi-cube-autosampler PUBLIC cxx_std_17)
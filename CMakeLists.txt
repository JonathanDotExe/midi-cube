cmake_minimum_required(VERSION 3.12)

project(midi-cube VERSION 1.0
    DESCRIPTION "A C++ synthesizer workstation"
    LANGUAGES CXX)


set(THREAD_PREFER_PTHREAD_FLAG ON)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(Threads REQUIRED)
find_package(rtaudio REQUIRED COMPONENTS rtaudio)
find_package(rtmidi REQUIRED COMPONENTS rtmidi)
find_package(sndfile REQUIRED COMPONENTS sndfile)
find_package(SFML REQUIRED COMPONENTS system window graphics)

file(GLOB_RECURSE FRAMEWORK_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/framework/*.cpp")
file(GLOB_RECURSE PLUGINS_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/plugins/*.cpp")
file(GLOB_RECURSE WORKSTATION_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/workstation/*.cpp")

add_library(midi-cube-framework ${FRAMEWORK_SOURCES})
target_link_libraries(midi-cube-framework PUBLIC SndFile::sndfile)
target_link_libraries(midi-cube-framework PUBLIC sfml-system)
target_link_libraries(midi-cube-framework PUBLIC sfml-window)
target_link_libraries(midi-cube-framework PUBLIC sfml-graphics)
target_link_libraries(midi-cube-framework PUBLIC Boost::filesystem)
target_link_libraries(midi-cube-framework PUBLIC Threads::Threads)

add_library(midi-cube-plugins ${PLUGINS_SOURCES})
target_link_libraries(midi-cube-plugins PUBLIC midi-cube-framework)

add_executable(midi-cube-workstation ${WORKSTATION_SOURCES} ${CMAKE_SOURCE_DIR} "midicube-main.cpp")
target_link_libraries(midi-cube-workstation PUBLIC midi-cube-framework)
target_link_libraries(midi-cube-workstation PUBLIC midi-cube-plugins)
target_link_libraries(midi-cube-workstation PUBLIC RtAudio::rtaudio)
target_link_libraries(midi-cube-workstation PUBLIC RtMidi::rtmidi)
target_include_directories(midi-cube-workstation PUBLIC include)

add_executable(midi-cube-autosampler ${WORKSTATION_SOURCES} ${CMAKE_SOURCE_DIR} "autosampler-main.cpp")
target_link_libraries(midi-cube-autosampler PUBLIC midi-cube-framework)
target_link_libraries(midi-cube-autosampler PUBLIC midi-cube-plugins)
target_link_libraries(midi-cube-autosampler PUBLIC RtAudio::rtaudio)
target_link_libraries(midi-cube-autosampler PUBLIC RtMidi::rtmidi)
target_include_directories(midi-cube-autosampler PUBLIC include)

target_compile_features(midi-cube-framework PUBLIC cxx_std_17)
target_compile_features(midi-cube-plugins PUBLIC cxx_std_17)
target_compile_features(midi-cube-workstation PUBLIC cxx_std_17)
target_compile_features(midi-cube-autosampler PUBLIC cxx_std_17)
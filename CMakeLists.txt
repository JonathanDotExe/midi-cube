cmake_minimum_required(VERSION 3.12)

project(midi-cube VERSION 1.0
    DESCRIPTION "A C++ synthesizer workstation"
    LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(THREAD_PREFER_PTHREAD_FLAG ON)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(Threads REQUIRED)
if (WIN32)
    find_package(rtaudio REQUIRED COMPONENTS rtaudio)
    find_package(rtmidi REQUIRED COMPONENTS rtmidi)
    find_package(sndfile REQUIRED COMPONENTS sndfile)
    find_package(SFML REQUIRED COMPONENTS system window graphics)
    
	set(CMAKE_CXX_STANDARD_LIBRARIES " -static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

file(GLOB_RECURSE FRAMEWORK_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/framework/*.cpp")
file(GLOB_RECURSE PLUGINS_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/plugins/*.cpp")
file(GLOB_RECURSE WORKSTATION_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "midicube/workstation/*.cpp")


add_library(midi-cube-framework ${FRAMEWORK_SOURCES})
target_link_libraries(midi-cube-framework PUBLIC sndfile)
target_link_libraries(midi-cube-framework PUBLIC sfml-system)
target_link_libraries(midi-cube-framework PUBLIC sfml-window)
target_link_libraries(midi-cube-framework PUBLIC sfml-graphics)
target_link_libraries(midi-cube-framework PUBLIC Boost::filesystem)
target_link_libraries(midi-cube-framework PUBLIC Threads::Threads)
target_include_directories(midi-cube-framework PUBLIC include)
add_library(midi-cube-plugins ${PLUGINS_SOURCES})
target_link_libraries(midi-cube-plugins PUBLIC midi-cube-framework)

add_executable(midi-cube-workstation ${WORKSTATION_SOURCES} ${CMAKE_SOURCE_DIR} "midicube-main.cpp")
target_link_libraries(midi-cube-workstation PUBLIC midi-cube-framework)
target_link_libraries(midi-cube-workstation PUBLIC midi-cube-plugins)
target_link_libraries(midi-cube-workstation PUBLIC rtaudio)
target_link_libraries(midi-cube-workstation PUBLIC rtmidi)
target_include_directories(midi-cube-workstation PUBLIC include)

add_executable(midi-cube-autosampler ${WORKSTATION_SOURCES} ${CMAKE_SOURCE_DIR} "autosampler-main.cpp")
target_link_libraries(midi-cube-autosampler PUBLIC midi-cube-framework)
target_link_libraries(midi-cube-autosampler PUBLIC midi-cube-plugins)
target_link_libraries(midi-cube-autosampler PUBLIC rtaudio)
target_link_libraries(midi-cube-autosampler PUBLIC rtmidi)
target_include_directories(midi-cube-autosampler PUBLIC include)

target_compile_features(midi-cube-framework PUBLIC cxx_std_17)
target_compile_features(midi-cube-plugins PUBLIC cxx_std_17)
target_compile_features(midi-cube-workstation PUBLIC cxx_std_17)
target_compile_features(midi-cube-autosampler PUBLIC cxx_std_17)

file(GLOB DLL_FILES ABSOLUTE "${CMAKE_SOURCE_DIR}/lib/*.dll")
file(GLOB_RECURSE RESOURCE_FILES RELATIVE ${CMAKE_SOURCE_DIR} "resources/*.*")

set(CMAKE_INSTALL_BINDIR ".")
set(CMAKE_INSTALL_LIBDIR ".")
install(TARGETS midi-cube-framework midi-cube-plugins midi-cube-workstation midi-cube-autosampler)
install(FILES ${DLL_FILES} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources/" DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)
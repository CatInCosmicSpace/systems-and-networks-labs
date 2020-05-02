cmake_minimum_required(VERSION 3.16)
project(labs)

set(CMAKE_CXX_STANDARD 17)

# === easylogging++ ===
include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
ExternalProject_Add(easyloggingpp
        GIT_REPOSITORY https://github.com/amrayn/easyloggingpp
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
file(GLOB easyloggingpp_src
        ${EXTERNAL_INSTALL_LOCATION}/include/easylogging++.h
        ${EXTERNAL_INSTALL_LOCATION}/include/easylogging++.cc
)


include_directories(common/)
file(GLOB common_src common/*.h common/*.cpp)

# === LAB1, LAB2 ===
file(GLOB single_frame_src single-frame/*.h single-frame/*.cpp)
add_executable(single_frame ${common_src} ${single_frame_src} ${easyloggingpp_src})
add_dependencies(single_frame easyloggingpp)

# === LAB3, LAB4, LAB5 ===
file(GLOB multi_frame_src multi-frame/*.h multi-frame/*.cpp)
add_executable(multi_frame ${common} ${multi_frame_src} ${easyloggingpp_src})
add_dependencies(multi_frame easyloggingpp)

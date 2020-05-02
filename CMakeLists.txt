cmake_minimum_required(VERSION 3.16)
project(labs)

set(CMAKE_CXX_STANDARD 17)

file(GLOB common_src common/*.h common/*.cpp)

# === LAB1, LAB2 ===
file(GLOB single_frame_src single-frame/*.h single-frame/*.cpp)
add_executable(single_frame ${common_src} ${single_frame_src})
target_include_directories(single_frame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# === LAB3, LAB4, LAB5 ===
file(GLOB multi_frame_src multi-frame/*.h multi-frame/*.cpp)
add_executable(multi_frame ${common_src} ${multi_frame_src})
target_include_directories(multi_frame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

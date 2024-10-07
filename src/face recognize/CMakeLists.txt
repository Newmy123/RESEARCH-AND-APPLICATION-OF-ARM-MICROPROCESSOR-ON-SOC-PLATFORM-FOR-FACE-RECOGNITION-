cmake_minimum_required(VERSION 3.8)
project(facere)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find Boost
find_package(Boost REQUIRED COMPONENTS filesystem system)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(facere facere.cpp)

# Link OpenCV libraries 
target_link_libraries(facere ${OpenCV_LIBS} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
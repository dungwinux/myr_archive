# CMakeList.txt : CMake project for myr_archive, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Strict error checking
if(MSVC)
    string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Include Boost
#set (Boost_NO_SYSTEM_PATHS ON)
#set (Boost_USE_MULTITHREADED ON)
#set (Boost_USE_STATIC_LIBS ON)
#set (Boost_USE_STATIC_RUNTIME OFF)
#set (BOOST_ALL_DYN_LINK OFF)
#find_package (Boost 1.77.0 REQUIRED COMPONENTS iostreams)

# Add source to this project's executable.
add_executable (myr_archive "myr_archive.cpp" "myr_archive.hpp" "main.cpp")
target_link_libraries(myr_archive)
# TODO: Add tests and install targets if needed.

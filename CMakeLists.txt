cmake_minimum_required(VERSION 3.5)

project(complex_numbers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_library(complex INTERFACE complex.cpp)

add_executable(complex_numbers main.cpp)
#target_link_libraries(complex_numbers complex)

install(TARGETS complex_numbers
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

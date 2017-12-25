cmake_minimum_required(VERSION 2.8)
project(bf)

if(MSVC)
    # Visual Studio -- /W4
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_GNUCC)
    # GCC -- -Wall -pedantic
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -pedantic -Wno-implicit-fallthrough")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pedantic -Wno-implicit-fallthrough")
endif()

add_executable(brainfuck bf.cc)
add_executable(brainfuckc bf.c)
# add_executable(brainfuckc bf.c)

install(TARGETS brainfuck DESTINATION bin)

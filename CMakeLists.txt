cmake_minimum_required(VERSION 2.8)
project(bf)

if(MSVC)
	# Visual Studio -- /W4
	if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
	endif()
elseif(CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_GNUCC)
	# GCC -- -Wall -pedantic
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif()

add_executable(brainfuck bf.c)

install(TARGETS brainfuck DESTINATION bin)

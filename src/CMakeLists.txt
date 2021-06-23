cmake_minimum_required(VERSION "3.18")
project("memgrind-c")

set(CXX_STANDARD "11")
set(CFLAGS "-Wall -Werror -pedantic-errors")

set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
set(CMAKE_CXX_FLAGS ${CFLAGS})

add_executable("memgrind-c" "memgrind_c.h" "memgrind_c.c")
target_link_libraries("memgrind-c" LINK_PUBLIC "cgcs_malloc" "cgcs_vector" "cgcs_ulog")

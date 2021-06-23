cmake_minimum_required(VERSION "3.18")
project("memgrind-c")

## submodules used by memgrind-c
add_subdirectory("./libs")

## memgrind-c client sources
add_subdirectory("./src")

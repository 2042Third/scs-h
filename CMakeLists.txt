cmake_minimum_required(VERSION 3.27)
project(lab1_15793 C)

set(CMAKE_C_STANDARD 11)

include_directories(prime_probe)

add_executable(lab1_15793
    prime_probe/params.h
    prime_probe/thief.c
    prime_probe/util.c
    prime_probe/util.h)

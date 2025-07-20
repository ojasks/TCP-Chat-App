cmake_minimum_required(VERSION 3.23)
project(SockTalk C)

set(CMAKE_C_STANDARD 99)

add_library(SocketUtil SHARED socketutil.c)
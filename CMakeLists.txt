cmake_minimum_required(VERSION 3.23)
project(SockTalk C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add subdirectories
add_subdirectory(SocketUtil)
add_subdirectory(SocketClient)
add_subdirectory(SocketServer)

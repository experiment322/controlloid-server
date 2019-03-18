cmake_minimum_required(VERSION 3.13)
project(ws_handler C)

set(CMAKE_C_STANDARD 99)

add_executable(ws_handler main.c)
target_link_directories(ws_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vjoy/lib)
target_link_libraries(ws_handler vJoyInterface)
target_include_directories(ws_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vjoy/inc)

install(TARGETS ws_handler RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/vjoy/lib/vJoyInterface.dll DESTINATION bin)

cmake_minimum_required(VERSION 3.13)
project(ws_handler C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EVDEV REQUIRED libevdev)

add_executable(ws_handler main.c)
target_link_libraries(ws_handler ${EVDEV_LIBRARIES})
target_include_directories(ws_handler PUBLIC ${EVDEV_INCLUDE_DIRS})

install(TARGETS ws_handler RUNTIME DESTINATION bin)

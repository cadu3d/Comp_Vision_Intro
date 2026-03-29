cmake_minimum_required(VERSION 3.16)
project(Comp_Vision_Intro)

set(CMAKE_CXX_STANDARD 20)

add_executable(Comp_Vision_Intro
    main.cpp
    lab1.cpp
    lab2.cpp
        utils.cpp
        utils.h
)

find_package(OpenCV REQUIRED)

target_link_libraries(Comp_Vision_Intro PRIVATE ${OpenCV_LIBS})
target_compile_definitions(Comp_Vision_Intro PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

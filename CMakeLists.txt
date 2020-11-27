cmake_minimum_required(VERSION 3.17)
project(BezierTest)

add_subdirectory(SDL)
add_subdirectory(glm)

add_executable(BezierTest main.cpp)

target_link_libraries(BezierTest SDL2-static SDL2main glm)
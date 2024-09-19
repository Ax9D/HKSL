cmake_minimum_required(VERSION 3.30)
project(HKSL)
set(CMAKE_CXX_STANDARD 20)

include_directories(include include/AST include/Parse include/Analysis)

file(GLOB all_sources CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/AST/*.cpp"
    "src/Analysis/*.cpp"
    "src/Parse/*.cpp"
)
add_executable(${PROJECT_NAME} ${all_sources})
target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME})
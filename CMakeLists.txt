cmake_minimum_required(VERSION 3.15)
project(schedulereader LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
	include/stb_ds.h
	src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

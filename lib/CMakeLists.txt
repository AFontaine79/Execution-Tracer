cmake_minimum_required(VERSION 3.18)
project(execution-tracer)

add_library(${PROJECT_NAME} OBJECT)

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/execution_tracer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

cmake_minimum_required(VERSION 3.18)
project(execution-tracer)

add_library(${PROJECT_NAME} OBJECT)

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

if (NOT EXEC_TRACE_CONF_FILE)
    message(FATAL_ERROR "Please set EXEC_TRACE_CONF_FILE to the location of execution_tracer_conf.h")
endif()

configure_file(${EXEC_TRACE_CONF_FILE} ${CUSTOM_INC_DIR}/execution_tracer_conf.h COPYONLY)

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/execution_tracer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CUSTOM_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

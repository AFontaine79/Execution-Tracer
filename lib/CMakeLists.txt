cmake_minimum_required(VERSION 3.18)
project(execution-tracer)

if (NOT DEFINED EXEC_TRACE_CONF_FILE)
    message(FATAL_ERROR "Please set EXEC_TRACE_CONF_FILE to the location of execution_tracer_conf.h")
endif()

if (NOT DEFINED EXEC_TRACE_LIBRARY_TYPE)
    set(EXEC_TRACE_LIBRARY_TYPE STATIC)
endif()

add_library(${PROJECT_NAME} ${EXEC_TRACE_LIBRARY_TYPE})

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

configure_file(${EXEC_TRACE_CONF_FILE} ${CUSTOM_INC_DIR}/execution_tracer_conf.h COPYONLY)

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/execution_tracer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CUSTOM_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

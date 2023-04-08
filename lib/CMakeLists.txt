cmake_minimum_required(VERSION 3.18)
project(execution-tracer)

if (NOT DEFINED EXEC_TRACE_CONF_FILE)
    set(EXEC_TRACE_CONF_FILE ${CMAKE_CURRENT_LIST_DIR}/include/execution_tracer_conf_template.h)
    set(EXEC_TRACE_FLASH_BASE 0x08000000 CACHE STRING "MCU base address for program code")
    set(EXEC_TRACE_RAM_BASE 0x20000000 CACHE STRING "MCU base address for RAM")
    set(EXEC_TRACE_SFR_BASE 0x40000000 CACHE STRING "MCU base address for memory-mapped peripheral register access")
    option(EXEC_TRACE_STOP_AFTER_RESET "Only trace up to the first reset" OFF)
    option(EXEC_TRACE_USE_NOINIT "Use noinit RAM for trace buffer and control structures" ON)
    option(EXEC_TRACE_ALLOW_OVERWRITE "Overwrite oldest entries when buffer is full" ON)
    set(EXEC_TRACE_BUFF_LENGTH_LIST 32 64 128 256 512 1024)
    set(EXEC_TRACE_NUM_TRACE_ENTRIES 128 CACHE STRING "Length of the trace buffer (multiply by 4 for size in bytes)")
    set_property(CACHE EXEC_TRACE_NUM_TRACE_ENTRIES PROPERTY STRINGS ${EXEC_TRACE_BUFF_LENGTH_LIST})
endif()

if (NOT DEFINED EXEC_TRACE_LIBRARY_TYPE)
    set(EXEC_TRACE_LIBRARY_TYPE STATIC)
endif()

add_library(${PROJECT_NAME} ${EXEC_TRACE_LIBRARY_TYPE})

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

configure_file(${EXEC_TRACE_CONF_FILE} ${CUSTOM_INC_DIR}/execution_tracer_conf.h)

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/execution_tracer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CUSTOM_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

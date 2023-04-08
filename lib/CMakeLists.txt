cmake_minimum_required(VERSION 3.18)
project(execution-tracer)

if (DEFINED EXEC_TRACE_CONF_FILE)
    # If a configuration file is provided to us, we will use it verbatim.
    set(CONFIGURE_FILE_EXTRA_ARGS COPYONLY)
else()
    # Otherwise, we will use the template configuration file and place all
    # configuration options in the CMake cache.
    set(EXEC_TRACE_FLASH_BASE 0x08000000 CACHE STRING "MCU base address for program code")
    set(EXEC_TRACE_RAM_BASE 0x20000000 CACHE STRING "MCU base address for RAM")
    set(EXEC_TRACE_SFR_BASE 0x40000000 CACHE STRING "MCU base address for memory-mapped peripheral register access")
    option(EXEC_TRACE_STOP_AFTER_RESET "Only trace up to the first reset" OFF)
    option(EXEC_TRACE_USE_NOINIT "Use noinit RAM for trace buffer and control structures" ON)
    option(EXEC_TRACE_ALLOW_OVERWRITE "Overwrite oldest entries when buffer is full" ON)
    set(EXEC_TRACE_BUFF_LENGTH_LIST 32 64 128 256 512 1024)
    set(EXEC_TRACE_NUM_TRACE_ENTRIES 128 CACHE STRING "Length of the trace buffer (multiply by 4 for size in bytes)")
    set_property(CACHE EXEC_TRACE_NUM_TRACE_ENTRIES PROPERTY STRINGS ${EXEC_TRACE_BUFF_LENGTH_LIST})

    set(CONFIGURE_FILE_EXTRA_ARGS)
    set(EXEC_TRACE_CONF_FILE ${CMAKE_CURRENT_LIST_DIR}/include/execution_tracer_conf_template.h)
endif()

# The output execution_tracer_conf.h file is placed in the build directory.
set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)
configure_file(${EXEC_TRACE_CONF_FILE} ${CUSTOM_INC_DIR}/execution_tracer_conf.h ${CONFIGURE_FILE_EXTRA_ARGS})

if (DEFINED EXEC_TRACE_LIBRARY_TYPE)
    # Allow execution tracer library type to be set explicitly. This is most
    # useful if using the OBJECT library type.
    add_library(${PROJECT_NAME} ${EXEC_TRACE_LIBRARY_TYPE})
else()
    # Build library according to the value of BUILD_SHARED_LIBS as set by the
    # developer. If not set, this will build as static.
    add_library(${PROJECT_NAME})
endif()

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/execution_tracer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CUSTOM_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

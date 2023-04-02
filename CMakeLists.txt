cmake_minimum_required(VERSION 3.18)

# Check that we are not the top-level directory.
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # If not, build the execution-tracer library target.
    add_subdirectory(lib)
else()
    # If so, warn of invalid use
    message(FATAL_ERROR "Execution Tracer only supports being added as a subproject")
endif()

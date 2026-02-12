# Copyright 2026 The MIND Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10)

# Project definition
project(mind
    VERSION 0.1.0
    DESCRIPTION "Deterministic cognitive runtime for persistent local memory"
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Warnings
    add_compile_options(-Wall -Wextra -Wpedantic)

    # Determinism: disable fast-math optimizations that could
    # introduce non-deterministic floating point behavior
    add_compile_options(-fno-fast-math -ffp-contract=off)

    # Debug flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()

    # Release flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O2)
    endif()
endif()

# MSVC flags
if(MSVC)
    add_compile_options(/W4 /fp:precise)
endif()

#=============================================================================
# Library
#=============================================================================

add_library(mind STATIC
    src/cr_runtime.c
    src/cr_state.c
    src/cr_query.c
    src/cr_temporal.c
    src/cr_persist.c
)

# Include directories
target_include_directories(mind
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link math library (required for sqrtf)
if(UNIX)
    target_link_libraries(mind PRIVATE m)
endif()

# Set library properties
set_target_properties(mind PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/cr.h
)

#=============================================================================
# Example
#=============================================================================

add_executable(mind_example examples/minimal.c)
target_link_libraries(mind_example PRIVATE mind)

#=============================================================================
# Tests
#=============================================================================

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # Basic test (runs the example)
    add_test(
        NAME example_runs
        COMMAND mind_example
    )
endif()

#=============================================================================
# Installation
#=============================================================================

include(GNUInstallDirs)

install(TARGETS mind
    EXPORT mind-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT mind-targets
    FILE mind-config.cmake
    NAMESPACE mind::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mind
)

#=============================================================================
# Package configuration
#=============================================================================

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    mind-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mind-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mind
)

#=============================================================================
# Summary
#=============================================================================

message(STATUS "")
message(STATUS "MIND v${PROJECT_VERSION}")
message(STATUS "")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  C compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests:    ${BUILD_TESTS}")
message(STATUS "")

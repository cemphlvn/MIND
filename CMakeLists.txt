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
    DESCRIPTION "Self-evolving cognitive runtime and memory layer for AI systems"
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
# FOUNDATION (Layer 0) - Pure math, never changes
#=============================================================================

add_library(mind_foundation STATIC
    foundation/src/mind_vec.c
)

target_include_directories(mind_foundation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/foundation/include>
        $<INSTALL_INTERFACE:include/mind>
)

if(UNIX)
    target_link_libraries(mind_foundation PRIVATE m)
endif()

#=============================================================================
# CORE (Layer 1) - Cognitive memory
#=============================================================================

add_library(mind_core STATIC
    core/src/cr_runtime.c
    core/src/cr_state.c
    core/src/cr_query.c
    core/src/cr_temporal.c
    core/src/cr_persist.c
)

target_include_directories(mind_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/include>
        $<INSTALL_INTERFACE:include/mind>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/foundation/include
)

target_link_libraries(mind_core PRIVATE mind_foundation)

#=============================================================================
# COMBINED LIBRARY (for convenience)
#=============================================================================

add_library(mind STATIC
    foundation/src/mind_vec.c
    core/src/cr_runtime.c
    core/src/cr_state.c
    core/src/cr_query.c
    core/src/cr_temporal.c
    core/src/cr_persist.c
)

target_include_directories(mind
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/foundation/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/include>
        $<INSTALL_INTERFACE:include/mind>
)

if(UNIX)
    target_link_libraries(mind PRIVATE m)
endif()

set_target_properties(mind PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
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

    add_executable(test_basic tests/test_basic.c)
    target_link_libraries(test_basic PRIVATE mind)

    add_test(NAME basic_tests COMMAND test_basic)
    add_test(NAME example_runs COMMAND mind_example)
endif()

#=============================================================================
# Installation
#=============================================================================

include(GNUInstallDirs)

install(TARGETS mind mind_foundation mind_core
    EXPORT mind-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    foundation/include/mind_vec.h
    core/include/cr.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mind
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
message(STATUS "  Layers:")
message(STATUS "    - foundation (Layer 0): Pure math")
message(STATUS "    - core (Layer 1):       Cognitive memory")
message(STATUS "")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  C compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests:    ${BUILD_TESTS}")
message(STATUS "")

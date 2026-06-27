cmake_minimum_required(VERSION 3.20)

project(DlineCore VERSION 0.1.0 LANGUAGES C)

add_library(dline_core_objects OBJECT)

target_sources(dline_core_objects
    PRIVATE
        src/buffer.c
        src/core.c
        src/memory.c
        src/utf8.c
)

target_compile_features(dline_core_objects
    PUBLIC c_std_17
)

set_target_properties(dline_core_objects PROPERTIES
    C_EXTENSIONS OFF
    C_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(dline_core_objects
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(dline_core_objects
    PRIVATE DLINE_CORE_EXPORTS
)

if(MSVC)
    target_compile_options(dline_core_objects PRIVATE /W4)
else()
    target_compile_options(
        dline_core_objects
        PRIVATE -Wall -Wextra -Wpedantic
    )
endif()

add_library(dline_core SHARED
    $<TARGET_OBJECTS:dline_core_objects>
)
add_library(Dline::Core ALIAS dline_core)

target_include_directories(dline_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(DLINE_BUILD_TESTS_DEFAULT ON)
else()
    set(DLINE_BUILD_TESTS_DEFAULT OFF)
endif()

option(
    DLINE_BUILD_TESTS
    "Build DlineCore tests"
    ${DLINE_BUILD_TESTS_DEFAULT}
)

if(DLINE_BUILD_TESTS)
    enable_testing()

    add_executable(dline_core_tests
        tests/main.c
    )

    target_link_libraries(dline_core_tests
        PRIVATE dline_core_objects
    )

    target_compile_definitions(dline_core_tests
        PRIVATE DLINE_CORE_EXPORTS
    )

    add_test(
        NAME dline_core_tests
        COMMAND dline_core_tests
    )

    add_executable(dline_public_api_tests
        tests/public_api.c
    )

    target_compile_features(dline_public_api_tests
        PRIVATE c_std_17
    )

    target_link_libraries(dline_public_api_tests
        PRIVATE Dline::Core
    )

    add_test(
        NAME dline_public_api_tests
        COMMAND dline_public_api_tests
    )
endif()

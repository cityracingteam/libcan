# Min CMake version
cmake_minimum_required(VERSION 3.19)

if (CAN_TARGET STREQUAL "pico")
    # Pull in pico sdk (must be before project() call)
    include(rp2040/pico-sdk.cmake)
endif()

project(
    libcan
    VERSION 0.0.1
    DESCRIPTION "can library"
    LANGUAGES CXX
)


if (CAN_TARGET STREQUAL "pico")
    pico_sdk_init()
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Tools


add_subdirectory(lib/boost/assert) # dependency of boost/conversion
add_subdirectory(lib/boost/config) # dependency of boost/conversion
add_subdirectory(lib/boost/conversion)
add_subdirectory(lib/boost/core) # dependency of boost/smart_ptr
add_subdirectory(lib/boost/preprocessor)
add_subdirectory(lib/boost/smart_ptr) # dependency of boost/conversion
add_subdirectory(lib/boost/static_assert) # dependency of boost/core
add_subdirectory(lib/boost/throw_exception) # dependency of boost/conversion

if (IS_TEST_ENV STREQUAL "true")
    add_subdirectory(lib/google/test)
    add_subdirectory(lib/google/gmock-global)
endif()

# Subprojects

add_subdirectory(core)

if (CAN_TARGET STREQUAL "pico")
    add_subdirectory(rp2040)
endif()

if (CAN_TARGET STREQUAL "linux")
    add_subdirectory(linux/socketcan)
endif()
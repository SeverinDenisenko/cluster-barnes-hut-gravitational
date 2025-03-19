cmake_minimum_required(VERSION 3.10)

project(cluster-barnes-hut-grav)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=thread")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-flto -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core-infrastructure)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core-math)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core-astronomy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core-threading)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core-algorithms)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cluster-networking)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cluster-application)

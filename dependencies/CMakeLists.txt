set(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spdlog)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/backward-cpp)

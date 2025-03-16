find_package(MPI REQUIRED)

add_library(cluster-networking STATIC cluster.cpp)

target_include_directories(cluster-networking PUBLIC include ${MPI_INCLUDE_PATH})
target_link_libraries(cluster-networking PUBLIC ${MPI_C_LIBRARIES})

if(MSVC)
    target_compile_options(cluster-networking PRIVATE /W4 /WX)
else()
    target_compile_options(cluster-networking PRIVATE -Wall -Wextra -Werror)
endif()

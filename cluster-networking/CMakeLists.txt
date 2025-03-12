add_library(cluster-networking STATIC cluster_networking.cpp)

target_include_directories(cluster-networking PUBLIC include)
target_link_libraries(cluster-networking PUBLIC )

if(MSVC)
    target_compile_options(cluster-networking PRIVATE /W4 /WX)
else()
    target_compile_options(cluster-networking PRIVATE -Wall -Wextra -Werror)
endif()

add_executable(cluster-application main.cpp)

target_link_libraries(cluster-application PRIVATE core-astronomy core-algorithms core-threading cluster-networking)

if(MSVC)
    target_compile_options(cluster-application PRIVATE /W4 /WX)
else()
    target_compile_options(cluster-application PRIVATE -Wall -Wextra -Werror)
endif()

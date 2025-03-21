add_executable(cluster-application main.cpp master.cpp slave.cpp frontend.cpp)

target_link_libraries(cluster-application PRIVATE core-astronomy core-threading cluster-networking raylib)

if (APPLE)
    target_link_libraries(cluster-application PRIVATE "-framework IOKit")
    target_link_libraries(cluster-application PRIVATE "-framework Cocoa")
    target_link_libraries(cluster-application PRIVATE "-framework OpenGL")
endif()

if(MSVC)
    target_compile_options(cluster-application PRIVATE /W4 /WX)
else()
    target_compile_options(cluster-application PRIVATE -Wall -Wextra -Werror)
endif()

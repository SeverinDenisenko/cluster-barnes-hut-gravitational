add_library(core-algorithms STATIC core_algorithms.cpp)

target_include_directories(core-algorithms PUBLIC include)
target_link_libraries(core-algorithms PUBLIC )

if(MSVC)
    target_compile_options(core-algorithms PRIVATE /W4 /WX)
else()
    target_compile_options(core-algorithms PRIVATE -Wall -Wextra -Werror)
endif()

add_library(core-algorithms STATIC tree.cpp)

target_include_directories(core-algorithms PUBLIC include)
target_link_libraries(core-algorithms PUBLIC core-math)

if(MSVC)
    target_compile_options(core-algorithms PRIVATE /W4 /WX)
else()
    target_compile_options(core-algorithms PRIVATE -Wall -Wextra -Werror)
endif()

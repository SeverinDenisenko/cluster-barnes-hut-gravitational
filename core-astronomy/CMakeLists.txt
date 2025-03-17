add_library(core-astronomy STATIC generator.cpp)

target_include_directories(core-astronomy PUBLIC include)
target_link_libraries(core-astronomy PUBLIC core-math)

if(MSVC)
    target_compile_options(core-astronomy PRIVATE /W4 /WX)
else()
    target_compile_options(core-astronomy PRIVATE -Wall -Wextra -Werror)
endif()

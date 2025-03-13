add_library(core-math STATIC linalg.cpp)

target_include_directories(core-math PUBLIC include)
target_link_libraries(core-math PUBLIC core-infrastructure)

if(MSVC)
    target_compile_options(core-math PRIVATE /W4 /WX)
else()
    target_compile_options(core-math PRIVATE -Wall -Wextra -Werror)
endif()

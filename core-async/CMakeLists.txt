add_library(core-async STATIC ev_loop.cpp)

target_include_directories(core-async PUBLIC include)
target_link_libraries(core-async PUBLIC core-infrastructure)

if(MSVC)
    target_compile_options(core-async PRIVATE /W4 /WX)
else()
    target_compile_options(core-async PRIVATE -Wall -Wextra -Werror)
endif()

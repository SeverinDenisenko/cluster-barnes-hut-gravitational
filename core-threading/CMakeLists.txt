add_library(core-threading STATIC core_threading.cpp)

target_include_directories(core-threading PUBLIC include)
target_link_libraries(core-threading PUBLIC )

if(MSVC)
    target_compile_options(core-threading PRIVATE /W4 /WX)
else()
    target_compile_options(core-threading PRIVATE -Wall -Wextra -Werror)
endif()

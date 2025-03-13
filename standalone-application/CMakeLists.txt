add_executable(standalone-application main.cpp)

target_link_libraries(standalone-application PRIVATE core-astronomy core-algorithms core-threading)

if(MSVC)
    target_compile_options(standalone-application PRIVATE /W4 /WX)
else()
    target_compile_options(standalone-application PRIVATE -Wall -Wextra -Werror)
endif()

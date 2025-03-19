add_library(core-infrastructure STATIC logging.cpp)

target_include_directories(core-infrastructure PUBLIC include)
target_link_libraries(core-infrastructure PUBLIC fmt::fmt yaml-cpp::yaml-cpp spdlog::spdlog Backward::Interface)

if(MSVC)
    target_compile_options(core-infrastructure PRIVATE /W4 /WX)
else()
    target_compile_options(core-infrastructure PRIVATE -Wall -Wextra -Werror)
endif()

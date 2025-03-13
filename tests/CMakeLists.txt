add_executable(quadtree-test quadtree_test.cpp)

target_link_libraries(quadtree-test PRIVATE core-algorithms gtest)

enable_testing()

add_test(NAME quadtree-test COMMAND quadtree-test)

if(MSVC)
    target_compile_options(quadtree-test PRIVATE /W4 /WX)
else()
    target_compile_options(quadtree-test PRIVATE -Wall -Wextra -Werror)
endif()

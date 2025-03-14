add_executable(quadtree-test quadtree_test.cpp)
add_executable(vector-test vector_test.cpp)

target_link_libraries(quadtree-test PRIVATE core-algorithms gtest)
target_link_libraries(vector-test PRIVATE core-math core-infrastructure gtest)

enable_testing()

add_test(NAME quadtree-test COMMAND quadtree-test)
add_test(NAME vector-test COMMAND vector-test)

if(MSVC)
    target_compile_options(quadtree-test PRIVATE /W4 /WX)
    target_compile_options(vector-test PRIVATE /W4 /WX)
else()
    target_compile_options(quadtree-test PRIVATE -Wall -Wextra -Werror)
    target_compile_options(vector-test PRIVATE -Wall -Wextra -Werror)
endif()

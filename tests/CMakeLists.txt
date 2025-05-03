add_executable(quadtree-test quadtree_test.cpp)
add_executable(vector-test vector_test.cpp)
add_executable(ev-loop-test ev_loop_test.cpp)

target_link_libraries(quadtree-test PRIVATE core-algorithms gtest)
target_link_libraries(vector-test PRIVATE core-math core-infrastructure gtest)
target_link_libraries(ev-loop-test PRIVATE core-async gtest)

enable_testing()

add_test(NAME quadtree-test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/quadtree-test)
add_test(NAME vector-test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vector-test)
add_test(NAME ev-loop-test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ev-loop-test)

if(MSVC)
    target_compile_options(quadtree-test PRIVATE /W4 /WX)
    target_compile_options(vector-test PRIVATE /W4 /WX)
    target_compile_options(ev-loop-test PRIVATE /W4 /WX)
else()
    target_compile_options(quadtree-test PRIVATE -Wall -Wextra -Werror)
    target_compile_options(vector-test PRIVATE -Wall -Wextra -Werror)
    target_compile_options(ev-loop-test PRIVATE -Wall -Wextra -Werror)
endif()

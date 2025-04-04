cmake_minimum_required(VERSION 3.29.3)

set(test_sources
	seg_fault.c
)

foreach(source IN LISTS test_sources)
	string(REPLACE ".c" "" test_names ${source})

	add_executable(${test_names} ${source})
	target_link_libraries(${test_names} ${shared_lib})
	add_test(NAME ${test_names} COMMAND ${test_names})
endforeach()

cmake_minimum_required(VERSION 3.29.3)

set(sources
	my_alloc.c
)

foreach(source_file IN LISTS sources)
	target_sources(${static_lib} PUBLIC ${source_file})
	target_sources(${shared_lib} PUBLIC ${source_file})
endforeach()

set(headers
	my_alloc.h
)

#get_target_property(static_headers ${static_lib} PUBLIC_HEADER)
#get_target_property(shared_headers ${shared_lib} PUBLIC_HEADER)

foreach(header_file IN LISTS headers)
	list(APPEND static_headers ${CMAKE_CURRENT_SOURCE_DIR}/${header_file})
	list(APPEND shared_headers ${CMAKE_CURRENT_SOURCE_DIR}/${header_file})
endforeach()

set_target_properties(${static_lib} PROPERTIES PUBLIC_HEADER "${static_headers}")
set_target_properties(${shared_lib} PROPERTIES PUBLIC_HEADER "${shared_headers}")


add_library(murmur3
		murmur3.c
		)

target_include_directories(murmur3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(murmur3::murmur3 ALIAS murmur3)

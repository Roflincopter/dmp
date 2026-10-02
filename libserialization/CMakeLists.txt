add_library(serialization INTERFACE)

target_include_directories(serialization
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(serialization
	INTERFACE Boost::headers
	INTERFACE Boost::serialization
)

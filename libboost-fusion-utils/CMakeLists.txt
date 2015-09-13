cmake_minimum_required(VERSION 2.8.12)

set(boost-fusion-utils-sources
	boost_tuple_serialization.hpp
	fusion_static_dispatch.hpp
	friendly_fusion.hpp
	fusion_outputter.hpp
	fusion_serializer.hpp
	fusion_lexicographical_compare.hpp
	index_list.hpp
	symbol.hpp symbol.cpp
)

add_library(boost-fusion-utils STATIC ${boost-fusion-utils-sources})

set_target_properties(boost-fusion-utils PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(boost-fusion-utils
	PRIVATE ${Boost_LIBRARIES}
)

target_include_directories(boost-fusion-utils SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(boost-fusion-utils
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(boost-fusion-utils 
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

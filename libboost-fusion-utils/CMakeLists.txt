add_library(boost-fusion-utils STATIC
	boost_tuple_serialization.hpp
	fusion_static_dispatch.hpp
	friendly_fusion.hpp
	fusion_outputter.hpp
	fusion_serializer.hpp
	fusion_lexicographical_compare.hpp
	index_list.hpp
	symbol.hpp symbol.cpp
)

target_include_directories(boost-fusion-utils
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(boost-fusion-utils
	PUBLIC Boost::headers
)

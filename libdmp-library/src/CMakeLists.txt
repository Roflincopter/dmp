cmake_minimum_required(VERSION 2.8.12)

file(GLOB dmp-library-sources *.cpp)
file(GLOB dmp-library-headers ../include/*.hpp)

add_library(dmp-library STATIC ${dmp-library-sources} ${dmp-library-headers})

target_include_directories(dmp-library SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE ${TAGLIB_INCLUDES}
	PRIVATE ${ICU_INCLUDE_DIRS}
)

target_include_directories(dmp-library
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(dmp-library
	PUBLIC boost-fusion-utils
	PUBLIC dmp-config
	PRIVATE serialization
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE ${TAGLIB_LIBRARIES}
	PRIVATE ${ICU_LIBRARIES}
)

if(USE_GPERF)
	target_link_libraries(dmp-library
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

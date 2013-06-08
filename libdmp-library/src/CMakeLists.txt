include_directories("../include")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

file(GLOB dmp-library-sources *.cpp)
file(GLOB dmp-library-headers ../include/*.hpp)

add_library(dmp-library SHARED ${dmp-library-sources} ${dmp-library-headers})

find_package(Boost COMPONENTS filesystem regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-library ${Boost_LIBRARIES})

find_package(TagLib REQUIRED)
target_link_libraries(dmp-library ${TAGLIB_LIBRARIES})

find_package(ICU REQUIRED)
include_directories(ICU_INCLUDE_DIRS)
target_link_libraries(dmp-library ${ICU_LIBRARIES})


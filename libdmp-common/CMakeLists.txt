CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-common-sources
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp
	message_outputter.hpp
	message_serializer.hpp
	message_callbacks.hpp
	gstreamer_base.hpp gstreamer_base.cpp
	fusion_static_dispatch.hpp
	friendly_fusion.hpp
	playlist.hpp
	message_switch.hpp
	debug_macros.hpp
	time_util.hpp time_util.cpp
	number_pool.hpp number_pool.cpp
	timed_debug.hpp timed_debug.cpp
)

if(MINGW)
	add_library(dmp-common STATIC ${dmp-common-sources})
else(MINGW)
	add_library(dmp-common SHARED ${dmp-common-sources})
endif(MINGW)

if(MINGW)
	target_link_libraries(dmp-common 
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstcoreelements.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgsttcp.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstplayback.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstvolume.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstaudioconvert.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstaudioresample.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstaudioparsers.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstautodetect.a
		/home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstlame.a
		-lmp3lame
		-lgsttag-1.0
		-lgstvideo-1.0
		-lgstpbutils-1.0
		-lgstaudio-1.0
		-lgio-2.0
		-ldnsapi
		-lgzip
		-lz
	)
endif(MINGW)

include_directories(../libdmp-library/include)

find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-common ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-common ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-common ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(SYSTEM ${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-common ${GOBJECT_LIBRARIES})

if(USE_GPERF)
	target_link_libraries(dmp-common "-lprofiler")
endif(USE_GPERF)

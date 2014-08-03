
#include_directories(../include)

add_executable(test1 test1.cpp)
target_link_libraries(test1
	PRIVATE dmp-library
)

add_executable(test2 test2.cpp)
target_link_libraries(test2
	PRIVATE dmp-library
)

add_executable(test3 test3.cpp)
target_link_libraries(test3
	PRIVATE dmp-library
)

add_executable(test4 test4.cpp)
target_link_libraries(test4
	PRIVATE dmp-library
)

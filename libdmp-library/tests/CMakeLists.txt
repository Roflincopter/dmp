
include_directories(../include)

add_executable(test1 test1.cpp)
target_link_libraries(test1 dmp-library)

add_executable(test2 test2.cpp)
target_link_libraries(test2 dmp-library)

include_directories(../include)

add_executable(test1 test1.cpp)
add_dependencies(test1 dmp-library)
target_link_libraries(test1 dmp-library)

cmake_minimum_required(VERSION 2.8.12)

# Must be one to resolve issues with boost::thread linking (turn off to see the issues)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_subdirectory(src)
add_subdirectory(tests)
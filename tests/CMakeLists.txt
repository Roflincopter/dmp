
add_executable(plaintext_connection_test plain_conn.cpp)

target_link_libraries(plaintext_connection_test
	PRIVATE dmp-net
)

add_test(testname plaintext_connection_test)
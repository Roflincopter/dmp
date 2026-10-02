add_executable(plaintext_connection_test plain_conn.cpp)
target_link_libraries(plaintext_connection_test
	PRIVATE dmp-net
	PRIVATE serialization
)
add_test(NAME plaintext_connection_test COMMAND plaintext_connection_test)

add_executable(encrypted_connection_test encr_conn.cpp)
target_link_libraries(encrypted_connection_test
	PRIVATE dmp-net
	PRIVATE serialization
)
add_test(NAME encrypted_connection_test COMMAND encrypted_connection_test)

add_executable(archive_test archive.cpp)
target_link_libraries(archive_test
	PRIVATE dmp-common
	PRIVATE boost-fusion-utils
	PRIVATE serialization
)
add_test(NAME archive_test COMMAND archive_test)

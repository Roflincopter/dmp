# Project-wide warning flags.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(
		-Wall -Wextra
		-Wno-missing-braces -Wno-missing-field-initializers
		-Wno-unused-parameter
	)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(
		-Wall -Wextra
		-Wno-missing-braces -Wno-missing-field-initializers
		-Wredundant-decls -Wlogical-op -Wformat=2 -Wpointer-arith
	)
elseif(MSVC)
	add_compile_options(/W4)
endif()

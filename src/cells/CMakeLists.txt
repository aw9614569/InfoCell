add_library(cellslib STATIC
Cells.cc
Cells.h
)
if(MSVC)
    target_compile_options(cellslib PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(cellslib
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(cellslib
	PUBLIC
		CONAN_PKG::utfcpp
		Boost::boost
)
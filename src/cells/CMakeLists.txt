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
		CONAN_PKG::boost
)

add_library(newcellslib STATIC
    NewCells.cc
    NewCells.h
    SVGPrinter.cc
)
if(MSVC)
    target_compile_options(newcellslib PUBLIC "/Zc:__cplusplus")
endif()
target_include_directories(newcellslib
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(newcellslib
	PUBLIC
		CONAN_PKG::utfcpp
		CONAN_PKG::boost
		CONAN_PKG::freetype
)


add_executable(newcelltest newcelltest.cc)
target_link_libraries(newcelltest
PRIVATE
    newcellslib
    applib
)
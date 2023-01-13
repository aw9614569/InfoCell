add_library(applib STATIC
    App.cc
    App.h
    Colors.h
    Picture.cc
    Picture.h
    Scroller.cc
    Scroller.h
    Solver.cc
    Solver.h
)

if(MSVC)
    target_compile_options(applib PUBLIC "/Zc:__cplusplus")
endif()

target_include_directories(applib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(applib
	PUBLIC
		CONAN_PKG::nlohmann_json
        CONAN_PKG::ftxui
	PRIVATE
		CONAN_PKG::utfcpp
        cellslib
        tests
        util
)
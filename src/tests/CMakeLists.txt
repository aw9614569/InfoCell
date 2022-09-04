add_library(tests STATIC
    UnitTester.cc
    UnitTester.h
)

target_link_libraries(tests
    cellslib
    util
)
cmake_minimum_required(VERSION 3.24)

# Project name
project(rosegarden)

include_directories(../src ${CMAKE_CURRENT_BINARY_DIR}/../src)

list(APPEND CMAKE_MODULE_PATH ../cmake)


if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_xml(NAME ${PROJECT_NAME}_coverage EXECUTABLE ${PROJECT_NAME})
endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"


# Code factorization: this macro defines unit tests
macro(RG_UNIT_TESTS)
   foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp)
      add_test(${_testname} ${CMAKE_CURRENT_BINARY_DIR}/${_testname})
      target_link_libraries(${_testname} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} rosegardenprivate)
      set_target_properties(${_testname} PROPERTIES COMPILE_FLAGS -DSRCDIR="\\"${CMAKE_CURRENT_SOURCE_DIR}/\\"")
   endforeach(_testname)
endmacro()

# Each line here defines a unit test (the executable name matches the .cpp filename)
RG_UNIT_TESTS(
   realtime
   accidentals
   segmenttransposecommand
   test_notationview_selection
   transpose
   reference_segment
   utf8
   testmisc
   convert
)

add_subdirectory(lilypond)


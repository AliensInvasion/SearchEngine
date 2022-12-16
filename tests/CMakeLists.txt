cmake_minimum_required(VERSION 3.11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SrcFiles
	${SRC_DIR}/InvertedIndex.cpp
	${SRC_DIR}/SearchEngine.cpp
)

set(TestsFiles
	"InvertedIndex_test.cpp"
	"SearchEngine_test.cpp"
)

add_executable(GtestTests ${TestsFiles} ${SrcFiles})
target_link_libraries(GtestTests nlohmann_json::nlohmann_json GTest::gtest_main)
target_include_directories(GtestTests PUBLIC ${INCLUDE_DIR})
include(GoogleTest)
gtest_discover_tests(GtestTests)


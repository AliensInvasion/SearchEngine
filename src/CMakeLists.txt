cmake_minimum_required(VERSION 3.11)

set(Src
	ConverterJSON.cpp
	SearchEngine.cpp
	InvertedIndex.cpp
	main.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(SearchEngine ${Src})

target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(SearchEngine PUBLIC ${INCLUDE_DIR})


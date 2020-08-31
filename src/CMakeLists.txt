

set(SOURCES
	compiler.cpp
	nodes.cpp
	runtime.cpp
	)

find_package(FLEX)
find_package(BISON 3.6.4 REQUIRED)

FLEX_TARGET(ECE467Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(ECE467Parser parser.y parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)

ADD_FLEX_BISON_DEPENDENCY(ECE467Lexer ECE467Parser)

add_library(ece467_lexer OBJECT ${FLEX_ECE467Lexer_OUTPUTS})
target_include_directories(ece467_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ece467_lexer PRIVATE ${CMAKE_BINARY_DIR}/src)

add_library(ece467_parser OBJECT ${BISON_ECE467Parser_OUTPUTS})
target_include_directories(ece467_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ece467_parser PRIVATE ${CMAKE_BINARY_DIR}/src)

find_package(LLVM 10.0 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core interpreter orcjit native)

add_library(ece467 STATIC ${SOURCES})
target_include_directories(ece467 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ece467 PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(ece467 PUBLIC -rdynamic)
target_link_libraries(ece467 PRIVATE ece467_lexer)
target_link_libraries(ece467 PRIVATE ece467_parser)
target_link_libraries(ece467 PUBLIC ${llvm_libs})

add_executable(ece467c
	main.cpp
	)
target_include_directories(ece467c PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ece467c PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(ece467c PUBLIC -rdynamic)
target_link_libraries(ece467c PUBLIC ece467)
target_include_directories(ece467c PRIVATE ${CMAKE_SOURCE_DIR}/src)

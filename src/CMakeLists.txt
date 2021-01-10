

set(SOURCES
	compiler.cpp
	common.cpp
	nodes.cpp
	vcodegen.cpp
	vevaluate.cpp
	vprint.cpp
	voptimize.cpp
	symtable.cpp
	)

find_package(FLEX)
find_package(BISON 3.6.4 REQUIRED)

FLEX_TARGET(cccLexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(cccParser parser.y parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)

ADD_FLEX_BISON_DEPENDENCY(cccLexer cccParser)

add_library(ccc_lexer OBJECT ${FLEX_cccLexer_OUTPUTS})
target_include_directories(ccc_lexer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ccc_lexer PRIVATE ${CMAKE_BINARY_DIR}/src)

add_library(ccc_parser OBJECT ${BISON_cccParser_OUTPUTS})
target_include_directories(ccc_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ccc_parser PRIVATE ${CMAKE_BINARY_DIR}/src)

find_package(LLVM 10.0 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core interpreter orcjit native)

add_library(cccl STATIC ${SOURCES})
target_include_directories(cccl PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(cccl PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(cccl PUBLIC -rdynamic)
target_link_libraries(cccl PRIVATE ccc_lexer)
target_link_libraries(cccl PRIVATE ccc_parser)
target_link_libraries(cccl PUBLIC ${llvm_libs})

add_executable(ccc
	main.cpp
	)
target_include_directories(ccc PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ccc PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(ccc PUBLIC -rdynamic)
target_link_libraries(ccc PUBLIC cccl)
target_include_directories(ccc PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_library(cccrt SHARED runtime.cpp)

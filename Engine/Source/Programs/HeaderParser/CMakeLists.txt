PROJECT(header-parser)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

find_package(OpenMP) 
if (OPENMP_FOUND) 
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}") 
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") 
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
set (CMAKE_CXX_STANDARD 20)

SET(SOURCES
  "main.cc"
  "options.h"
  "token.h"
  "tokenizer.cc"
  "tokenizer.h"
  "parser.cc"
  "parser.h"
  "type_node.h"
   "Tasks/FillContent.cc" "Tasks/FillContent.h")

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/external/rapidjson/include"
	"${PROJECT_SOURCE_DIR}/external/tclap/include"
	)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
   ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  add_definitions(-std=c++20)
endif()
ADD_EXECUTABLE(header-parser ${SOURCES} parser.cc parser.h main.h)

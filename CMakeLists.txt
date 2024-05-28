cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX "d")
else()
    set(CMAKE_DEBUG_POSTFIX "")
endif()

project ("PythonCpp")
add_library(PythonCpp)

set_property(TARGET PythonCpp PROPERTY CXX_STANDARD 17)

find_path(PYTHON_INCLUDE_DIR NAMES Python.h REQUIRED)

find_package(Python REQUIRED Development)

target_include_directories(PythonCpp PRIVATE "${PYTHON_INCLUDE_DIR}")
target_include_directories(PythonCpp PRIVATE "include")
target_include_directories(PythonCpp PRIVATE "src")

target_link_libraries(PythonCpp PUBLIC Python::Python)

add_subdirectory ("src")
add_subdirectory ("include")
add_subdirectory ("example")
add_subdirectory ("tests")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PythonCppExample)

set(
	CMAKE_INSTALL_LIBDIR
	"${CMAKE_INSTALL_PREFIX}/lib"
	CACHE PATH "Specify path where to install library files"
)

set(
	CMAKE_INSTALL_INCLUDEDIR
	"${CMAKE_INSTALL_PREFIX}/include"
	CACHE PATH "Specify path where to install header files"
)

set(
	CMAKE_INSTALL_PDBDIR
	"${CMAKE_INSTALL_PREFIX}/bin"
	CACHE PATH "Specify path where to install PDB files"
)

install(
	FILES $<TARGET_FILE:PythonCpp>
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
	DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING PATTERN "*.h"
)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/PythonCpp.dir/PythonCpp.pdb"
	DESTINATION ${CMAKE_INSTALL_PDBDIR}
	OPTIONAL
)
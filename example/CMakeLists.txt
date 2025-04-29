cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PythonCppExample")

add_executable(PythonCppExample)

set_property(TARGET PythonCppExample PROPERTY CXX_STANDARD 17)

find_package(PythonCpp CONFIG REQUIRED)

target_include_directories(PythonCppExample PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(PythonCppExample PRIVATE "include")

target_link_libraries(PythonCppExample PRIVATE PythonCpp)

add_subdirectory ("src")
add_subdirectory ("include")

add_custom_command(
  TARGET PythonCppExample POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${PROJECT_SOURCE_DIR}/python" "${PROJECT_BINARY_DIR}/python"
)

#include("${CMAKE_SOURCE_DIR}/cmake/InstallPython.cmake")

if(${PythonCpp_FOUND})
	include("${PYCPP_INSTALL_PYTHON}")
	InstallPython(PACKAGES "pyglm==2.8.1" HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/pythondefs.h")
	target_sources(PythonCppExample PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/pythondefs.h")
else()
	message(WARNING "PythonCpp is not installed. Python installation script may not be run unless PythonCpp is installed. Use 'sudo make install' to install it, then reconfigure cmake.")
endif()

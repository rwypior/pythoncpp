﻿cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PythonCppExample")

add_executable(PythonCppExample)

set_property(TARGET PythonCppExample PROPERTY CXX_STANDARD 17)

target_include_directories(PythonCppExample PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(PythonCppExample PRIVATE "include")

target_link_libraries(PythonCppExample PRIVATE PythonCpp)

add_subdirectory ("src")
add_subdirectory ("include")

add_custom_command(
  TARGET PythonCppExample POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${PROJECT_SOURCE_DIR}/python" "${PROJECT_BINARY_DIR}/python"
)
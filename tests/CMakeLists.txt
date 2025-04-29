cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PythonCppTest")

add_executable(PythonCppTest)
set_property(TARGET PythonCppTest PROPERTY CXX_STANDARD 17)

find_package(Catch2 3.6.0 REQUIRED)

target_include_directories(PythonCppTest PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(PythonCppTest PRIVATE "include")

target_link_libraries(PythonCppTest PRIVATE PythonCpp)
target_link_libraries(PythonCppTest PRIVATE Catch2::Catch2WithMain)

add_subdirectory ("src")
add_subdirectory ("include")

add_custom_command(
  TARGET PythonCppTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${PROJECT_SOURCE_DIR}/python" "${PROJECT_BINARY_DIR}/python"
)

include("${CMAKE_SOURCE_DIR}/cmake/InstallPython.cmake")
InstallPython(PACKAGES "pyglm==2.8.1" HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/pythondefs.h")
target_sources(PythonCppExample PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/pythondefs.h")

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(PythonCppTest)

cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_DEBUG_POSTFIX "d")

project ("PythonCpp")
add_library(PythonCpp)
add_library(PythonCpp::PythonCpp ALIAS PythonCpp)

set_property(TARGET PythonCpp PROPERTY CXX_STANDARD 17)

find_package(Python REQUIRED Development)

target_include_directories (PythonCpp PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(PythonCpp PRIVATE "src")

target_link_libraries(PythonCpp PUBLIC Python::Python)

add_subdirectory ("src")
add_subdirectory ("include/pythoncpp")
add_subdirectory ("example")

if(${BUILD_TESTING})
    message("Building unit tests")

    include(CTest)
    add_subdirectory ("tests")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PythonCppExample)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ConfigPackageLocation lib/cmake/PythonCpp)

configure_package_config_file(
        cmake/PythonCppConfig.cmake.in
	    "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp/PythonCppConfig.cmake"
	    INSTALL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp"
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp/PythonCppConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
)

install(
        TARGETS PythonCpp EXPORT PythonCppTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY "include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h"
)

#export(EXPORT PythonCppTargets
#        FILE "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp/PythonCppTargets.cmake"
#        NAMESPACE PythonCpp::
#)

install(EXPORT PythonCppTargets
        FILE PythonCppTargets.cmake
        NAMESPACE PythonCpp::
        DESTINATION ${ConfigPackageLocation}
)

install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp/PythonCppConfig.cmake" 
            "${CMAKE_CURRENT_BINARY_DIR}/pythonCpp/PythonCppConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation}
)

install(FILES
			"cmake/InstallPython.cmake"
			"scripts/installpython.py"
			DESTINATION "${ConfigPackageLocation}/util"
)

export(PACKAGE PythonCpp)

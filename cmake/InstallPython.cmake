function(InstallPython)
	set(options "DRYRUN")
	set(onevalueargs "VERSION" "SCRIPTPATH" "PACKAGEDIR" "INSTALLDIR" "LIBDIR" "TMP" "HEADER")
	set(multivalueargs "PACKAGES" "IGNORE")
	cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${onevalueargs}" "${multivalueargs}")
	
	if(NOT arg_LIBDIR)
		set(arg_LIBDIR "dist/lib")
	endif()

	if(NOT arg_TMP)
		set(arg_TMP "${CMAKE_CURRENT_BINARY_DIR}/tmp")
	endif()

	if(NOT arg_SCRIPTPATH)
		if(DEFINED PYCPP_INSTALL_PYTHON_PY)
			# For installed installpython.py
			set(arg_SCRIPTPATH "${PYCPP_INSTALL_PYTHON_PY}")
		else()
			# Fallback for using local installpython.py
			set(arg_SCRIPTPATH "${CMAKE_SOURCE_DIR}/scripts/installpython.py")
		endif()
	endif()

	if(NOT arg_INSTALLDIR)
		set(arg_INSTALLDIR "${CMAKE_CURRENT_BINARY_DIR}")
	endif()

	find_package(Python ${arg_VERSION} COMPONENTS Interpreter Development)
	if(Python_FOUND)
		set(pycpp_package_str "")
		foreach(lib IN LISTS arg_PACKAGES)
			set(pycpp_package_str "${pycpp_package_str} ${lib}")
		endforeach()
		
		if(NOT ${pycpp_package_str} STREQUAL "")
			set(pycpp_package_str "-p ${pycpp_package_str}")
		endif()
		string(REPLACE " " ";" pycpp_package_str "${pycpp_package_str}")

		set(pycpp_ignore_str "")
		foreach(ign IN LISTS arg_IGNORE)
			set(pycpp_ignore_str "${pycpp_ignore_str} ${ign}")
		endforeach()
		
		if(NOT ${pycpp_ignore_str} STREQUAL "")
			set(pycpp_ignore_str "-e ${pycpp_ignore_str}")
		endif()
		string(REPLACE " " ";" pycpp_ignore_str "${pycpp_ignore_str}")

		set(pycpp_DRYRUN "")
		if(arg_DRYRUN)
			set(pycpp_DRYRUN "-d")
		endif()

		set(pycpp_PACKAGEDIR "")
		if(NOT ${arg_PACKAGEDIR} STREQUAL "")
			set(pycpp_PACKAGEDIR "--installdir ${arg_PACKAGEDIR}")
			string(REPLACE " " ";" pycpp_PACKAGEDIR "${pycpp_PACKAGEDIR}")
		endif()

		set(pycpp_HEADER "")
		if(NOT ${arg_HEADER} STREQUAL "")
			set(pycpp_HEADER "--generateheader ${arg_HEADER}")
			string(REPLACE " " ";" pycpp_HEADER "${pycpp_HEADER}")
		endif()
				
		execute_process(COMMAND ${Python_EXECUTABLE} ${arg_SCRIPTPATH} ${arg_INSTALLDIR} -a ${arg_LIBDIR} ${pycpp_package_str} ${pycpp_ignore_str} ${pycpp_PACKAGEDIR} --wheeldir ${arg_TMP} ${pycpp_HEADER} ${pycpp_DRYRUN} -s -i)
	else()
		message("Python not found. In order to install Python environment, please install Python")
	endif()
endfunction()

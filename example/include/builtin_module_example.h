/*
* Basic Python example loading built-in module
* Load a module, execute a function with and display the result
*/

#pragma once

#include "common.h"
#include "pythondefs.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct BuiltinModuleExample : public IExample
{
	virtual std::string getExampleName() const override { return "Built-in module example"; }
	virtual void run()
	{
		std::string pydir = getPythonDirectory();
		std::string distdir = getDistDirectory();

		Python::Initializer::getDefaultInitializer().setHome(distdir);

		// Create Python instance
		Python::Python py;
		py.setPath(pydir);
		py.appendPath(distdir);

		// Load the module
		Python::Module site = py.loadModule("site");

		// Get a function from the module
		Python::Function getusersitepackages = site.getFunction("getusersitepackages");

		std::cout << "Calling \"" << getusersitepackages.getName() << "\" function." << std::endl << std::endl;

		// Call the function
		Python::Object result = getusersitepackages();

		std::cout << result.toString() << std::endl;
	}
};

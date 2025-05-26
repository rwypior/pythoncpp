/*
* Basic Python example
* Load a module, execute a function with two arguments and display the result
*/

#pragma once

#include "common.h"
#include "pythondefs.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct BasicExample : public IExample
{
	virtual std::string getExampleName() const override { return "Basic example"; }
	virtual void run()
	{
		// Initialize python environment
		Python::Initializer::getDefaultInitializer().setHome(Python::pythonhome);

		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		// Load the module
		Python::Module basic_example = py.loadModule("basic_example");

		std::cout << "Loaded \"" << basic_example.getName() << "\" module. Here's some doc:\n" << basic_example.getDocstring() << std::endl << std::endl;

		// Get a function from the module
		Python::Function basic_sum = basic_example.getFunction("basic_sum");

		std::cout << "Calling \"" << basic_sum.getName() << "\" function. Here's some doc:\n" << basic_sum.getDocstring() << std::endl << std::endl;

		// Call the function and get result
		Python::Object result = basic_sum(1337, 42);

		std::cout << "basic_example.basic_sum(1337, 42) = " << result.toLong() << std::endl;
	}
};
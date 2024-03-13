/*
* Basic Python example
* Load a module, execute a function with two arguments and display the result
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <PythonCpp.h>

#include <iostream>

struct BasicExample : public IExample
{
	virtual std::string getExampleName() const override { return "Basic example"; }
	virtual void run()
	{
		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		// Load the module
		Python::Module basic_example = py.loadModule("basic_example");

		// Get a function from the module
		Python::Function basic_sum = basic_example.getFunction("basic_sum");

		// Call the function and get result
		Python::Object result = basic_sum(1337, 42);

		std::cout << "basic_example.basic_sum(1337, 42) = " << result.toLong() << std::endl;
	}
};
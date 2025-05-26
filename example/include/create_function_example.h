/*
* Create Python function from source code
* Load a module, create the function and call it
*/

#pragma once

#include "common.h"
#include "pythondefs.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct CreateFunctionExample : public IExample
{
	virtual std::string getExampleName() const override { return "Create function from source"; }
	virtual void run()
	{
		// Initialize python environment
		Python::Initializer::getDefaultInitializer().setHome(Python::pythonhome);

		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		// Load the module
		Python::Module basic_example = py.loadModule("basic_example");

		// Create the function
		constexpr char examplePythonFunctionName[] = "created_function";
		constexpr char examplePythonFunction[] = R"(def created_function():
    print("This is printed from Python function"))";

		Python::Function createdFunction = basic_example.createFunction(examplePythonFunctionName, examplePythonFunction);

		// Call the function
		createdFunction.call();
	}
};
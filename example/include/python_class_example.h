/*
* Basic Python class usage example
* Load a module, get a class, create an instance of the class and call a function of the instance
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>
#include <exception>

struct PythonClassExample : public IExample
{
	virtual std::string getExampleName() const override { return "Python class example"; }
	virtual void run()
	{
		try
		{
			// Create Python instance
			Python::Python py;
			py.appendPath(getPythonDirectory());

			// Load the module
			Python::Module basic_example = py.loadModule("basic_example");

			// Get Python class
			Python::Class example_class = basic_example.getClass("example_class");

			std::cout << "Loaded \"" << example_class.getName() << "\" class. Here's some doc:\n" << example_class.getDocstring() << std::endl << std::endl;

			// Create class instance
			Python::Instance example_class_instance = example_class.createInstance();

			// Call a function from the instance
			Python::Object result = example_class_instance.call("hello", { 1337, 42 });

			std::cout << "example_class.hello(1337, 42) = " << result.toLong() << std::endl;
		}
		catch (const std::exception& ex)
		{
			std::cout << "Error:" << std::endl;
			std::cout << ex.what() << std::endl;
		}
	}
};

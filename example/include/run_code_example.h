/*
* Run code example
* Run some Python code as it was a terminal
*/

#pragma once

#include "common.h"
#include "pythondefs.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct RunCodeExample : public IExample
{
	virtual std::string getExampleName() const override { return "Run code"; }
	virtual void run()
	{
		// Initialize python environment
		Python::Initializer::getDefaultInitializer().setHome(Python::pythonhome);

		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		try
		{
			Python::Module mod = py.getModule("__main__");
			//Python::Module mod("mymod");
			mod.run("print('Hello!')");
			mod.run("print(__name__)");

			mod.run("import basic_example");
			mod.run("print(basic_example.basic_sum(40, 2))");
		}
		catch (const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
};
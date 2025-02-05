/*
* Example of reloading Python modules
* Load a module, execute a code, reload a module and execute the code again
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct ModuleReloadingExample : public IExample
{
	void callFunction(Python::Module& module)
	{
		// Get a function from the module
		Python::Function call_imported_module = module.getFunction("call_imported_module");

		// Call the function
		call_imported_module();
	}

	virtual std::string getExampleName() const override { return "Module reloading example"; }
	virtual void run()
	{
		try
		{
			// Create Python instance
			Python::Python py;
			py.appendPath(getPythonDirectory());

			// Load the module
			Python::Module module_example_b = py.loadModule("module_example_b");

			callFunction(module_example_b);

			std::cout << "Now change the code, and press any key..." << std::endl;
			std::cin.clear(); std::cin.ignore(INT_MAX, '\n');
			std::cin.get();

			module_example_b.reload();

			callFunction(module_example_b);
		}
		catch (const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
};
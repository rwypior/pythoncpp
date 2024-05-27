/*
* Create Python module example
* Create Python module with a single method, load existing Python module,
* call existing Python module function which utilizes newly created module
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <PythonCpp.h>

#include <iostream>


// Create a function which will be available in the newly created module
// The function takes two parameters:
// pySelf - instance of a module in which the function resides
// pyArgs - a tuple of arguments passed to the function
// The function's return value will be passed to Python
// void pointers in this function can be converted to and from Python::Object
// and it's subclasses
inline void* cppFunc(void* pySelf, void* pyArgs)
{
	Python::Tuple args(pyArgs);
	Python::Object d = args[0];
	std::string str = d.toString();

	std::cout << "cppFunc received '" << str << "'" << std::endl;

	static long num = 1337;

	Python::Object result = Python::Object(num);

	num *= 2;

	return result;
}

struct CreateModuleExample : public IExample
{
	virtual std::string getExampleName() const override { return "Create module"; }
	virtual void run()
	{
		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		// Create module builder
		Python::ModuleBuilder builder({
			// Add previously prepared function to the module
			// name, pointer to function, flags, documentation string
			{ "cppFunc", cppFunc, Python::DefaultFunctionFlag, "Example C++ function" }
		});

		// Register new module
		Python::Module myMod = py.registerModule("cppModule");

		// Use ModuleBuilder to fill new module with functions
		builder.build(myMod);

		// Load Python module and call a function which calls cppFunc from cppModule
		Python::Module basic_example = py.loadModule("module_example");
		Python::Function call_cpp_func = basic_example.getFunction("call_cpp_func");
		call_cpp_func.call();
	}
};
/*
* Usage example for various Python elements
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <pythoncpp/PythonCpp.h>

#include <iostream>

struct UsageExample : public IExample
{
	virtual std::string getExampleName() const override { return "Python usage example"; }
	virtual void run()
	{
		// Create Python instance
		// Note - even though no part of 'py' variable will be used - it's still required to initialize
		// Python itself, which is done in Python::Python class
		Python::Python py;

		// 
		Python::List empty_list = Python::List();
		for (const auto& element : empty_list)
		{
			std::cout << element.toString() << std::endl;
		}
	}
};
/*
* Create Python class example
* First create a Python module, and then create a Python class and embed it into
* newly created module, next load existing Python module which calls a method
* which uses newly created class
*/

#pragma once

#include "common.h"

// Include all-in-one PythonCPP header
#include <PythonCpp.h>

#include <iostream>

// Create a structure containing class members
// Structures may only contain primitive types, including raw pointers
struct PythonClass
{
	unsigned int counter;
};

// Function called when the class is instantiated
static void* classNew(void* type, void* args, void* kwds)
{
	std::cout << "Class is created!" << std::endl;

	// Call Python::initializeClass to allocate the memory for the new class
	PythonClass* self = Python::initializeClass<PythonClass>(type);

	// Set initial value of members
	self->counter = 0;

	// Return newly allocated class
	return Python::baseObject(self);
}

// Function called when the class is destroyed
static void classDelete(void* type)
{
	std::cout << "Class is destroyed!" << std::endl;
}

// Function called when the class is initialized,
// this is equivalent to Python's __init__ method
static int classInit(void* self, void* args, void* kwds)
{
	std::cout << "Class is initialized!" << std::endl;
	return 0;
}

// Regular member function of the class
inline void* classSpinCounter(void* pySelf, void* pyArgs)
{
	// Call Python::getDataStructure to get the instance of class'
	// structure
	PythonClass& data = Python::getDataStructure<PythonClass>(pySelf);

	// Do some operations on data
	Python::Object result = data.counter;
	++data.counter;

	// Return some result
	return result;
}

struct CreateClassExample : public IExample
{
	virtual std::string getExampleName() const override { return "Create class"; }
	virtual void run()
	{
		// Create Python instance
		Python::Python py;
		py.appendPath(getPythonDirectory());

		// Register new module
		// If the module should contain some functions, it's possible
		// to use ModuleBuilder here
		Python::Module myMod = py.registerModule("cppModule");

		// Create type builder
		Python::TypeBuilder typeBuilder({
			Python::Types::ClassDefinition("CppClass")
				// Specify used data structure of the class
				.setType<PythonClass>()

				.setDocString("Example C++ class")

				// Specify some special functions
				.setInitFunction(classInit)
				.setNewFunction(classNew)
				.setDeleteFunction(classDelete)

				// Add member functions
				.addFunction({"spinCounter", classSpinCounter, Python::DefaultFunctionFlag, "Spin the counter!"})

				// Register class member - this should be done for every field present in class' data structure
				.addMemberDefinition({"counter", Python::Types::MemberDefinition::Type::Long, offsetof(PythonClass, counter)})
			});
		typeBuilder.buildInto(myMod);

		// Create instance of newly created class
		Python::Class cppClass = myMod.getClass("CppClass");
		Python::Instance cppClassInstance = cppClass.createInstance();

		// Load Python module and call a function which takes new class instance as a parameter
		Python::Module basic_example = py.loadModule("class_example");
		Python::Function visit_cpp_class = basic_example.getFunction("visit_cpp_class");

		// Class function are called directly on the instance
		// Take a note there are 'call' and 'callref' function available
		// 'call' function makes a copy of the object, and 
		// 'callref' does not
		visit_cpp_class.callref({ cppClassInstance });
		visit_cpp_class.callref({ cppClassInstance });
		visit_cpp_class.callref({ cppClassInstance });

		// Access class data
		PythonClass& data = Python::getDataStructure<PythonClass>(cppClassInstance);
		std::cout << "Final counter value is " << data.counter << std::endl;
	}
};
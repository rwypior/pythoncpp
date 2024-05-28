#include "PythonCppCommon.hpp"
#include "PythonBuilders.h"
#include "PythonModule.h"

namespace Python
{
	// Module builder

	ModuleBuilder::ModuleBuilder(std::initializer_list<Types::FunctionDefinition> defs)
		: functionDefinitions(defs)
	{
	}

	ModuleBuilder& ModuleBuilder::addFunction(const Types::FunctionDefinition& def)
	{
		this->functionDefinitions.push_back(def);
		return *this;
	}

	void ModuleBuilder::build(Module& module) const
	{
		module.addFunctions(this->functionDefinitions);
	}

	// Type builder

	TypeBuilder::TypeBuilder(std::initializer_list<Types::ClassDefinition> defs)
		: classDefinitions(defs)
	{
	}

	TypeBuilder& TypeBuilder::addMethod()
	{
		return *this;
	}

	void TypeBuilder::buildInto(Module& module)
	{
		module.addClasses(this->classDefinitions);
	}
}
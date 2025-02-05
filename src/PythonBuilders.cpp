#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonBuilders.h"
#include "pythoncpp/PythonModule.h"

namespace Python
{
	void* defaultFlushFunc(void* /*self*/, void* /* arg */)
	{
		return Object::none();
	}

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

	void ModuleBuilder::registerPrintFunction(FunctionDefinition::fncPtr printFunc, FunctionDefinition::fncPtr flushFunc)
	{
		this->addFunction({ "write", printFunc, DefaultFunctionFlag, "" });
		this->addFunction({ "flush", flushFunc ? flushFunc : &defaultFlushFunc, DefaultFunctionFlag, "" });
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
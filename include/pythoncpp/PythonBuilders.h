#ifndef _h_python_builders
#define _h_python_builders

#include "pythoncpp/PythonDefs.h"

#include <initializer_list>
#include <vector>

namespace Python
{
	class Module;

	struct FunctionDefinition
	{
		typedef void* (*fncPtr)(void* pySelf, void* pyArgs);

		std::string name;
		fncPtr function;
		std::string docString;

		FunctionDefinition(const std::string& name, fncPtr function, const std::string& docString = "")
			: name(name)
			, function(function)
			, docString(docString)
		{ }
	};

	class ModuleBuilder
	{
		friend class Module;
	public:
		ModuleBuilder(std::initializer_list<Types::FunctionDefinition> defs = {});
		ModuleBuilder& addFunction(const Types::FunctionDefinition& def);

		void registerPrintFunction(FunctionDefinition::fncPtr printFunc, FunctionDefinition::fncPtr flushFunc = nullptr);

		void build(Module& module) const;

	protected:
		std::vector<Types::FunctionDefinition> functionDefinitions;
	};


	struct ClassDefinition
	{
		std::string name;
		std::string docString;

		ClassDefinition(const std::string& name)
			: name(name)
		{
		}
	};

	class TypeBuilder
	{
		friend class Module;
	public:
		TypeBuilder(std::initializer_list<Types::ClassDefinition> defs = {});

		TypeBuilder& addMethod();

		void buildInto(Module& module);

	protected:
		std::vector<Types::ClassDefinition> classDefinitions;
	};
}

#endif
#ifndef _h_python_module
#define _h_python_module

#include "pythoncpp/PythonDefs.h"
#include "pythoncpp/PythonDict.h"

#include <memory>
#include <vector>

namespace Python
{
	class Function;
	class Object;
	class Dict;

	class Module
	{
		friend class Dict;
		friend class Python;

	public:
		Module(pyobj module, std::shared_ptr<Dict> globals = std::make_shared<Dict>());
		Module(const std::string& moduleName);

		Module(const Module& module) = delete;
		Module(Module&& module);

		~Module();

		Module& operator=(Module&& b);

		operator pyobj() const;

		void reload();

		Function createFunction(const std::string& name, const std::string& source);
		Function getFunction(const std::string& name);
		void addFunctions(const std::vector<Types::FunctionDefinition>& definitions);

		Class getClass(const std::string& name);
		Object getVariable(const std::string& name);
		void setVariable(const std::string& name, const Object &variable = Object::none());
		void addClasses(const std::vector<Types::ClassDefinition>& definitions);

		void pushConstant(const std::string& name, const std::string& value);
		void pushImport(const std::string& name, Module mod);

		void run(const std::string& code);

		std::string getName() const;
		std::string getDocstring() const;
		bool exists(const std::string& name) const;
		std::string getPath() const;

	protected:
		pyobj module;

	private:
		std::shared_ptr<Dict> locals;
		std::shared_ptr<Dict> globals;
		std::unique_ptr<PythonFunctionDefContainer> functions;
		std::unique_ptr<PythonClassDefContainer> classes;
	};

	void* getDataStructureImpl(void* self);

	template<typename T>
	inline T& getDataStructure(void* self)
	{
		return *static_cast<T*>(getDataStructureImpl(self));
	}

	void* initializeClassImpl(void* type);

	template<typename T>
	inline T* initializeClass(void* type)
	{
		return static_cast<T*>(initializeClassImpl(type));
	}

	void* baseObject(void* self);
}

#endif
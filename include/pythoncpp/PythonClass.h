#ifndef _h_python_class
#define _h_python_class

#include "pythoncpp/PythonDefs.h"
#include "pythoncpp/PythonObject.h"

namespace Python
{
	class Instance;
	class Function;

	class Class
	{
		friend class Instance;
		friend class Module;
	public:
		Class(pyobj cls);

		Instance createInstance(std::vector<Object> params = {}) const;

		bool contains(const std::string& name) const;

		Function getFunction(const std::string& name) const;

		std::string getDocstring() const;
		std::string getName() const;

		pyobj getType() const;
		bool isHeapType() const;

	protected:
		pyobj cls;
	};
}

#endif
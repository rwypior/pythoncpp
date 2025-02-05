#ifndef _h_python_instance
#define _h_python_instance

#include "pythoncpp/PythonDefs.h"
#include "pythoncpp/PythonObject.h"

#include <vector>

namespace Python
{
	class Class;

	class Instance : public Object
	{
		friend class Class;

	public:
		Instance() = default;
		Instance(const Instance& inst);
		Instance(Instance&& inst);
		Instance(Object&& data);
		Instance(const Object& data);

		Instance& operator=(Instance&& inst);

		Object call(const std::string& func) const;
		Object call(const std::string& func, Object params) const;
		Object call(const std::string& func, const std::vector<Object>& params) const;
		Object call(const std::string& func, const std::initializer_list<Object>& params) const;

		Object getVariable(const std::string& name);
		void setVariable(const std::string& name, Object variable);

		bool contains(const std::string& name) const;

	protected:
		Instance(const Class& cls, std::vector<Object> params = {});

		static Object createNew(const Class &cls, const std::vector<Object> &params);
	};
}

#endif
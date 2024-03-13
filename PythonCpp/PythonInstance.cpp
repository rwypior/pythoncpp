#include "private/Python.hpp"
#include "PythonInstance.h"
#include "PythonTuple.h"
#include "PythonClass.h"

namespace Python
{
	// Ref data setter

	class InstanceSetter : public Setter
	{
	public:
		InstanceSetter(Instance& instance, const std::string& varName)
			: instance(instance)
			, varName(varName)
		{
		}

		void set(const Object& data) override
		{
			this->instance.setVariable(this->varName, data);
		}

	private:
		Instance& instance;
		std::string varName;
	};

	// Instance

	Object Instance::createNew(const Class& cls, const std::vector<Object> &params)
	{
		Tuple tuple(params);
		return Object(PyObject_CallObject(PO(cls.cls), PO(tuple)));
	}

	Instance::Instance(const Instance& inst)
		: Object(deepcopy(PO(inst)))
	{
	}

	Instance::Instance(Instance&& inst)
		: Object(inst.data)
	{
		inst.data = nullptr;
	}

	Instance::Instance(Object&& data)
		: Object(std::move(data))
	{
		data.data = nullptr;
	}

	Instance::Instance(const Object& data)
		: Object(PO(data))
	{
	}

	Instance::Instance(const Class& cls, std::vector<Object> params)
		: Object(createNew(cls, params))
	{
	}

	Instance& Instance::operator=(Instance&& inst)
	{
		this->data = std::move(inst.data);
		inst.data = nullptr;
		return *this;
	}

	Object Instance::call(const std::string& func) const
	{
		PyObject* result = PyObject_CallMethod(PO(this->data), func.c_str(), NULL);
		checkError();
		return Object(result);
	}

	Object Instance::call(const std::string& func, Object params) const
	{
		PyObject* keywords = PyDict_New();
		PyObject* method = PyObject_GetAttrString(PO(this->data), func.c_str());
		PyObject* result = PyObject_Call(method, PO(params.data), keywords);
		checkError();
		return Object(result);
	}

	Object Instance::call(const std::string& func, const std::vector<Object>& params) const
	{
		Tuple tuple(params);
		PyObject* keywords = PyDict_New();
		checkError();

		PyObject* method = PyObject_GetAttrString(PO(this->data), func.c_str());
		checkError();

		PyObject* result = PyObject_Call(method, PO(tuple.data), keywords);
		checkError();

		return Object(result);
	}

	Object Instance::call(const std::string& func, const std::initializer_list<Object>& params) const
	{
		Tuple tuple(params);
		PyObject* keywords = PyDict_New();
		checkError();

		PyObject* method = PyObject_GetAttrString(PO(this->data), func.c_str());
		checkError();

		PyObject* result = PyObject_Call(method, PO(tuple.data), keywords);
		checkError();

		return Object(result);
	}

	bool Instance::contains(const std::string& name) const
	{
		bool ret = PyObject_HasAttrString(PO(this->data), name.c_str());
		checkError();
		return ret;
	}

	Object Instance::getVariable(const std::string& name)
	{
		PyObject* var = PyObject_GetAttrString(PO(this->data), name.c_str());
		checkError();
		return Object(var, new InstanceSetter(*this, name));
	}

	void Instance::setVariable(const std::string& name, Object variable)
	{
		PyObject_SetAttrString(PO(this->data), name.c_str(), PO(variable));
		checkError();
	}
}
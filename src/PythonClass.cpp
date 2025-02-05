#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonClass.h"
#include "pythoncpp/PythonInstance.h"
#include "pythoncpp/PythonFunction.h"

namespace Python
{
	Class::Class(pyobj cls)
		: cls(cls)
	{
	}

	Instance Class::createInstance(std::vector<Object> params) const
	{
		return Instance(*this, params);
	}

	bool Class::contains(const std::string& name) const
	{
		bool ret = PyObject_HasAttrString(PO(this->cls), name.c_str());
		checkError();
		return ret;
	}

	Function Class::getFunction(const std::string& name) const
	{
		PyObject* function = PyObject_GetAttrString(PO(this->cls), name.c_str());
		checkError();

		return Function(function);
	}

	std::string Class::getDocstring() const
	{
		Object obj = Object(PyObject_GetAttrString(PO(this->cls), "__doc__"));
		return obj.toString();
	}

	std::string Class::getName() const
	{
		Object obj = Object(PyObject_GetAttrString(PO(this->cls), "__name__"));
		return obj.toString();
	}

	pyobj Class::getType() const
	{
		pyobj ret = PyObject_Type(PO(this->cls));
		checkError();
		return ret;
	}

	bool Class::isHeapType() const
	{
		PyTypeObject* type = reinterpret_cast<PyTypeObject*>(this->cls);
		return type->tp_flags & Py_TPFLAGS_HEAPTYPE;
	}
}
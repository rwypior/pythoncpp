#include "private/Python.hpp"
#include "PythonClass.h"
#include "PythonInstance.h"
#include "PythonFunction.h"

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
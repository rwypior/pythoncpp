#include "private/Python.hpp"
#include "PythonFunction.h"
#include "PythonTuple.h"

namespace Python
{
	Function::Function(pyobj function)
		: function(function)
	{
	}

	Function::~Function()
	{
		Py_DECREF(this->function);
	}

	Object Function::call()
	{
		PyObject* result = PyObject_CallObject(PO(this->function), nullptr);
		checkError();
		return Object(result);
	}

	Object Function::call(const Tuple &params)
	{
		PyObject* result = PyObject_CallObject(PO(this->function), PO(params.data));
		checkError();
		return Object(result);
	}

	Object Function::call(std::vector<Object> params)
	{
		Tuple tuple(params);
		return this->call(tuple);
	}

	Object Function::call_v(std::vector<Object> params)
	{
		return this->call(params);
	}

	Object Function::callref(std::vector<std::reference_wrapper<Object>> params)
	{
		Tuple tuple(params);
		return this->call(tuple);
	}
}
#include "PythonCppCommon.hpp"
#include "PythonMain.h"
#include "PythonException.h"
#include "PythonModule.h"
#include "PythonFunction.h"
#include "PythonList.h"
#include "PythonTuple.h"

namespace Python
{
	inline std::string formatError2(PyObject* type, PyObject* values, PyObject* trace)
	{
		PyObject* tracebackStr = PyUnicode_FromString("traceback");
		PyObject* traceback = PyImport_Import(tracebackStr);
		Py_DECREF(tracebackStr);

		PyObject* formatException = PyObject_GetAttrString(traceback, "format_exception");
		PyObject* res = PyObject_CallFunctionObjArgs(formatException, type, values, trace, NULL);
		std::string formatted = pyobj_to_string(res);

		return formatted;
	}

	inline std::string getErrorString(PyObject* type, PyObject* values)
	{
		std::string result;
		result += pyobj_to_string(type) + ": ";
		result += pyobj_to_string(values);
		return result;
	}

	inline std::string formatError(PyObject* type, PyObject* values, PyObject* trace)
	{
		if (!trace)
			return getErrorString(type, values);

		static bool guard = false;
		if (guard)
		{
			guard = false;
			return getErrorString(type, values);
		}
		guard = true;

		Python &py = Python::getInstance();
		Module traceback = py.loadModule("traceback");
		Function formatException = traceback.getFunction("format_exception");
		Tuple args(std::vector<pyobj> { type, values, trace });
		List list = formatException.call(args);

		std::string result;
		for (Object d : list)
		{
			result += d.toString() + "\n";
		}

		guard = false;
		return result;
	}

	void checkError()
	{
		if (PyErr_Occurred())
		{
			PyObject* type;
			PyObject* values;
			PyObject* trace;

			PyErr_Fetch(&type, &values, &trace);
			PyErr_NormalizeException(&type, &values, &trace);

			throw PythonErrorException(type, values, trace);
		}
	}

	// Exceptions

	PythonErrorException::PythonErrorException(pyobj type, pyobj values, pyobj trace)
		: PythonException(formatError(PO(type), PO(values), PO(trace)))
		, type(type)
		, values(values)
		, trace(trace)
	{
	}

	std::string PythonErrorException::getExceptionType() const
	{
		return pyobj_to_string(this->type);
	}

	std::string PythonErrorException::getExceptionValue() const
	{
		return pyobj_to_string(this->values);
	}
}
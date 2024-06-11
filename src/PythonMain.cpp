#include "PythonCppCommon.hpp"
#include "PythonMain.h"
#include "PythonModule.h"
#include "PythonList.h"
#include "PythonBuilders.h"
#include "PythonException.h"

#include <cstdlib>
#include <cassert>
#include <stdlib.h>

namespace Python
{
	std::string makeEnv(const std::string& name, const std::string& value)
	{
		return name + "=" + value;
	}

	void setenv(const std::string &name, const std::string& value)
	{
#pragma warning( push )
#pragma warning( disable: 4996 )
		int res = putenv(makeEnv(name, value).c_str());
		assert(res == 0);
		(void)res;
#pragma warning( pop )
	}

	// Defs

	constexpr size_t objsize()
	{
		return sizeof(PyObject);
	}

	// Initializer

	Initializer Initializer::defaultInitializer;

	Initializer& Initializer::getDefaultInitializer()
	{
		return defaultInitializer;
	}

	Initializer& Initializer::setHome(const std::string& home)
	{
		this->envs[PythonHome] = home;
		return *this;
	}

	std::string Initializer::getHome() const
	{
		auto it = this->envs.find(PythonHome);
		if (it != this->envs.end())
			return it->second;
		return "";
	}

	// Python

	Python::Python(const Initializer& initializer)
	{
		setenv(PythonHome, initializer.getHome());

		Py_Initialize();
		checkError();
	}

	Python::~Python()
	{
		if (Py_IsInitialized())
		{
			checkError();
			Py_Finalize();
		}
	}

	Python& Python::getInstance()
	{
		static Python py;
		return py;
	}

	void Python::setArgs(std::vector<std::wstring> argv)
	{
		std::vector<wchar_t*> wargv;
		for (std::wstring& str : argv)
		{
			wargv.push_back(&str[0]);
		}
		PySys_SetArgvEx(static_cast<int>(argv.size()), wargv.data(), 0);
		checkError();
	}

	void Python::setArgs(std::vector<std::string> argv)
	{
		std::vector<wchar_t*> wargv;
		for (std::string& str : argv)
		{
			std::wstring wstr = conv_string(str);
			wargv.push_back(&wstr[0]);
		}
		PySys_SetArgvEx(static_cast<int>(argv.size()), wargv.data(), 0);
		checkError();
	}

	List Python::getPath() const
	{
		return List(PySys_GetObject("path"));
	}

	void Python::setPath(const List& list)
	{
		PySys_SetObject("path", PO(list));
	}

	void Python::setPath(const std::string& path)
	{
		List l;
		l.pushItem(path);
		this->setPath(l);
	}

	void Python::appendPath(const std::wstring& path)
	{
		PyObject* syspath = PySys_GetObject("path");
		PyList_Append(syspath, PyUnicode_FromWideChar(path.c_str(), path.size()));
		checkError();
	}

	void Python::appendPath(const std::string& path)
	{
		this->appendPath(conv_string(path));
	}

	std::string Python::getHome() const
	{
		wchar_t *res = Py_GetPythonHome();

		if (res)
			return conv_string(res);

		return "";
	}

	void Python::setHome(const std::wstring& path) const
	{
		Py_SetPythonHome(path.c_str());
	}

	void Python::setHome(const std::string& path) const
	{
		this->setHome(conv_string(path));
	}

	Module Python::loadModule(const std::string& path)
	{
		PyObject* pymod = PyImport_ImportModule(path.c_str());
		checkError();

		return Module(pymod);
	}

	Module Python::reloadModule(Module& module)
	{
		PyObject *obj = PyImport_ReloadModule(PO(module));
		checkError();
		return Module(obj);
	}

	Module Python::registerModule(const std::string& name)
	{
		PyObject* mod = PyImport_AddModule(name.c_str());
		checkError();

		return Module(mod);
	}

	Module Python::getModule(const std::string& name)
	{
		PyObject* pypath = PyUnicode_FromString(name.c_str());
		checkError();

		PyObject* mod = PyImport_GetModule(pypath);
		checkError();

		Py_DECREF(pypath);
		checkError();

		return Module(mod);
	}

	void Python::redirectOutput(const Module& module, const std::string& streamName)
	{
		assert(
			"Printing module should define 'write' and 'flush' functions to be able to print correctly. "
			"Use 'registerPrintFunction' function on ModuleBuilder to register the functions."
			&& 
			module.exists("write") && module.exists("flush")
		);

		PySys_SetObject(streamName.c_str(), PO(module));
		checkError();
	}

	void Python::flushStdout()
	{
		PyObject* pystdout = PySys_GetObject("stdout");
		PyObject* strflush = PyUnicode_FromString("flush");
		PyObject_CallMethodNoArgs(pystdout, strflush);
		checkError();
	}

	void Python::flushStderr()
	{
		PyObject* pystdout = PySys_GetObject("stderr");
		PyObject* strflush = PyUnicode_FromString("flush");
		PyObject_CallMethodNoArgs(pystdout, strflush);
		checkError();
	}

	void Python::flush()
	{
		this->flushStdout();
		this->flushStderr();
	}

	void Python::checkErrors()
	{
		checkError();
	}
}
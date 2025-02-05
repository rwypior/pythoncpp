#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonInspect.h"
#include "pythoncpp/PythonInstance.h"
#include "pythoncpp/PythonMain.h"
#include "pythoncpp/PythonModule.h"
#include "pythoncpp/PythonFunction.h"
#include "pythoncpp/PythonList.h"

namespace Python
{
	constexpr char InspectModuleName[] = "inspect";

	Module getInspectModule()
	{
		Python& py = Python::getInstance();
		
		if (Module mod = py.getModule(InspectModuleName))
			return mod;

		return py.loadModule(InspectModuleName);
	}

	Inspect::Inspect()
		: inspectModule(getInspectModule())
	{
	}

	Module Inspect::getModule(pyobj data)
	{
		Function getmodule = this->inspectModule.getFunction("getmodule");
		pyobj res = getmodule.call_v({ Object(data) });
		Module mod = Module(res);
		return mod;
	}

	Module Inspect::getModule(Function data)
	{
		return this->getModule(data.function);
	}
}
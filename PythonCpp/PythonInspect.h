#ifndef _h_python_inspect
#define _h_python_inspect

#include "PythonDefs.h"
#include "PythonModule.h"

namespace Python
{
	class Inspect
	{
	public:
		Inspect();

		Module getModule(pyobj data);
		Module getModule(Function data);

	private:
		Module inspectModule;
	};
}

#endif
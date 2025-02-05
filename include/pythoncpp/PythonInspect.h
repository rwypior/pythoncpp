#ifndef _h_python_inspect
#define _h_python_inspect

#include "pythoncpp/PythonDefs.h"
#include "pythoncpp/PythonModule.h"

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
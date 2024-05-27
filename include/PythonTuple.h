#ifndef _h_python_tuple
#define _h_python_tuple

#include "PythonDefs.h"
#include "PythonObject.h"

#include <vector>
#include <functional>

namespace Python
{
	class Tuple : public Object
	{
	public:
		Tuple(pyobj tuple);
		Tuple(const Tuple& tuple);
		Tuple(std::vector<Object> tuple);
		Tuple(std::vector<std::reference_wrapper<Object>> tuple);
		Tuple(std::vector<pyobj> tuple);

		Tuple(Object&& data) noexcept;
		Tuple(Tuple&& tuple) noexcept;
		
		explicit Tuple(size_t elementCount);

		size_t getSize() const;

		Object operator[](unsigned int idx);
	};
}

#endif
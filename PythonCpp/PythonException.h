#ifndef _h_python_exception
#define _h_python_exception

#include "PythonDefs.h"

namespace Python
{
	class PythonException : public std::exception
	{
	public:
		PythonException(const std::string& msg)
			: std::exception(msg.c_str())
		{ }
	};

	class PythonErrorException : public PythonException
	{
	public:
		PythonErrorException(pyobj type, pyobj values, pyobj trace);

		std::string getExceptionType() const;
		std::string getExceptionValue() const;

	private:
		pyobj type;
		pyobj values;
		pyobj trace;
	};

	class PythonGeneralException : public PythonException
	{
	public:
		PythonGeneralException(const std::string& msg)
			: PythonException(msg.c_str())
		{ }
	};

	class PythonOutOfBoundsException : public PythonException
	{
	public:
		PythonOutOfBoundsException(const std::string& msg)
			: PythonException(msg.c_str())
		{ }
	};
}

#endif
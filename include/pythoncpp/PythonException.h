#ifndef _h_python_exception
#define _h_python_exception

#include "pythoncpp/PythonDefs.h"

namespace Python
{
	class PythonException : public std::exception
	{
	public:
		PythonException(const std::string& msg)
			: message(msg)
		{ }

		const char* what() const noexcept override
		{
			return this->message.c_str();
		}

	private:
		const std::string& message;
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

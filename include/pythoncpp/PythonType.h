#ifndef _h_python_type
#define _h_python_type

#include "pythoncpp/PythonDefs.h"
#include "pythoncpp/PythonObject.h"

namespace Python
{
	class Type : public Object
	{
	public:
		static constexpr char TypeNone[] = "NoneType";
		static constexpr char TypeString[] = "str";
		static constexpr char TypeInt[] = "int";
		static constexpr char TypeFloat[] = "float";
		static constexpr char TypeList[] = "list";
		static constexpr char TypeDict[] = "dict";
		static constexpr char TypeFunction[] = "function";
		static constexpr char TypeType[] = "type";
		static constexpr char TypeModule[] = "module";

	public:
		explicit Type(const pyobj& data);

		std::string toString() const;

		bool isNone() const;
		bool isString() const;
		bool isInt() const;
		bool isFloat() const;
		bool isList() const;
		bool isDict() const;
		bool isFunction() const;
		bool isType() const;
		bool isModule() const;
		bool operator==(const Type& b) const;
	};
}

#endif
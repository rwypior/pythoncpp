#include "private/Python.hpp"
#include "PythonType.h"

namespace Python
{
	Type::Type(const pyobj& data)
		: Object(PyObject_Type(PO(data)))
	{
		checkError();
	}

	std::string Type::toString() const
	{
		return PT(this->data)->tp_name;
	}

	bool Type::isNone() const
	{
		return this->toString() == TypeNone;
	}

	bool Type::isString() const
	{
		return this->toString() == TypeString;
	}

	bool Type::isInt() const
	{
		return this->toString() == TypeInt;
	}

	bool Type::isFloat() const
	{
		return this->toString() == TypeFloat;
	}

	bool Type::isList() const
	{
		return this->toString() == TypeList;
	}

	bool Type::isDict() const
	{
		return this->toString() == TypeDict;
	}

	bool Type::isFunction() const
	{
		return this->toString() == TypeFunction;
	}

	bool Type::isType() const
	{
		return this->toString() == TypeType;
	}

	bool Type::isModule() const
	{
		return this->toString() == TypeModule;
	}

	bool Type::operator==(const Type& b) const
	{
		return this->toString() == b.toString();
	}
}
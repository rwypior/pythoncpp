// Private

#ifndef _h_python_util
#define _h_python_util

#include "pythoncpp/PythonDefs.h"

#ifdef _DEBUG
#	define RESTORE_DEBUG
#	undef _DEBUG
#endif
#include <Python.h>
#include <structmember.h>
#ifdef RESTORE_DEBUG
#	undef RESTORE_DEBUG
#	define _DEBUG
#endif

#include <string>
#include <locale>
#include <codecvt>

namespace Python
{
	inline std::string conv_string(const std::wstring& wstr)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		return conv.to_bytes(wstr);
	}

	inline std::wstring conv_string(const std::string& wstr)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		return conv.from_bytes(wstr);
	}

	inline PyObject* PO(pyobj o)
	{
		return static_cast<PyObject*>(o);
	}

	inline PyTypeObject* PT(pyobj o)
	{
		return static_cast<PyTypeObject*>(o);
	}

	inline std::string py_str(PyObject* obj)
	{
		PyObject* enc = PyUnicode_AsEncodedString(obj, "utf-8", "strict");
		const char* bytes = PyBytes_AS_STRING(enc);

		Py_XDECREF(enc);

		return std::string(bytes);
	}

	inline std::string pyobj_to_string(pyobj obj)
	{
		PyObject* str = PyObject_Str(PO(obj));
		std::string res = py_str(str);
		Py_XDECREF(str);
		return res;
	}

	inline std::string pyobj_repr(pyobj obj)
	{
		PyObject* str = PyObject_Repr(PO(obj));
		std::string res = py_str(str);
		Py_XDECREF(str);
		return res;
	}

	void checkError();

	inline PyObject* deepcopy(PyObject* obj)
	{
		// Deepcopy may return an identical object if an object passed
		// is a primitive type. This may be similar to Python's caching
		// for small types, eg two separate variables with a small value
		// like '42' may be references to exactly same place

		if (!obj)
			return nullptr;

		assert(Py_REFCNT(obj) >= 1);
		PyGILState_STATE state = PyGILState_Ensure();

		PyObject* pypath = PyUnicode_FromString("copy");
		PyObject* pymod = PyImport_Import(pypath);
		checkError();

		Py_DECREF(pypath);

		checkError();

		PyObject* function = PyObject_GetAttrString(pymod, "deepcopy");
		checkError();

		PyObject* args = PyTuple_Pack(1, obj);
		checkError();

		PyObject* result = PyObject_CallObject(function, args);
		checkError();

		PyGILState_Release(state);

		return result;
	}

	inline const char* toUtf8(const std::string& str)
	{
		PyObject* obj = PyUnicode_FromString(str.c_str());
		const char* chars = PyUnicode_AsUTF8(obj);
		// TODO - obj needs to be disposed
		return chars;
	}
	
	// TODO
	// Change this according to PY_MAJOR_VERSION and PY_MINOR_VERSION
	inline int translateType(Types::MemberDefinition::Type type)
	{
		switch (type)
		{
		case Types::MemberDefinition::Type::Short: return T_SHORT;
		case Types::MemberDefinition::Type::Int: return T_INT;
		case Types::MemberDefinition::Type::Long: return T_LONG;
		case Types::MemberDefinition::Type::Float: return T_FLOAT;
		case Types::MemberDefinition::Type::Double: return T_DOUBLE;
		case Types::MemberDefinition::Type::String: return T_STRING;
		case Types::MemberDefinition::Type::Object: return T_OBJECT;
		case Types::MemberDefinition::Type::ObjectEx: return T_OBJECT_EX;
		case Types::MemberDefinition::Type::Char: return T_CHAR;
		case Types::MemberDefinition::Type::Byte: return T_BYTE;
		case Types::MemberDefinition::Type::Ubyte: return T_UBYTE;
		case Types::MemberDefinition::Type::Uint: return T_UINT;
		case Types::MemberDefinition::Type::Ushort: return T_USHORT;
		case Types::MemberDefinition::Type::Ulong: return T_LONG;
		case Types::MemberDefinition::Type::Bool: return T_BOOL;
		case Types::MemberDefinition::Type::Longlong: return T_LONGLONG;
		case Types::MemberDefinition::Type::Ulonglong: return T_ULONGLONG;
		case Types::MemberDefinition::Type::Pysizet: return T_PYSSIZET;
		}

		return 0;
	}
}

#endif
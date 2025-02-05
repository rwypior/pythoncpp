#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonTuple.h"
#include "pythoncpp/PythonException.h"

namespace Python
{
	// Ref data setter

	class TupleSetter : public Setter
	{
	public:
		TupleSetter(Object& container, const unsigned int key)
			: container(container)
			, key(key)
		{
		}

		void set(const Object& data) override
		{
			PyTuple_SET_ITEM(PO(this->container), this->key, PO(data));
			Py_INCREF(PO(data));
			checkError();
		}

	private:
		Object& container;
		const unsigned int key;
	};

	// Tuple

	Tuple::Tuple(pyobj tuple)
		: Object(tuple)
	{
	}

	Tuple::Tuple(const Tuple& tuple)
		: Object(deepcopy(PO(tuple.data)))
	{
	}

	Tuple::Tuple(std::vector<Object> tuple)
		: Tuple(tuple.size())
	{
		for (unsigned int i = 0; i < tuple.size(); i++)
		{
			PyTuple_SET_ITEM(PO(this->data), i, PO(tuple[i]));
			checkError();
		}
	}

	Tuple::Tuple(std::vector<std::reference_wrapper<Object>> tuple)
		: Tuple(tuple.size())
	{
		for (unsigned int i = 0; i < tuple.size(); i++)
		{
			Object& data = tuple[i].get();
			PyTuple_SET_ITEM(PO(this->data), i, PO(data));
			checkError();
		}
	}

	Tuple::Tuple(std::vector<pyobj> tuple)
		: Tuple(tuple.size())
	{
		for (unsigned int i = 0; i < tuple.size(); i++)
		{
			PyTuple_SET_ITEM(PO(this->data), i, PO(tuple[i]));
			checkError();
		}
	}

	Tuple::Tuple(Object&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	Tuple::Tuple(Tuple&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	Tuple::Tuple(size_t elementCount)
		: Object(PyTuple_New(elementCount))
	{
		checkError();
	}

	size_t Tuple::getSize() const
	{
		size_t ret = PyTuple_Size(PO(this->data));
		checkError();
		return ret;
	}

	Object Tuple::operator[](unsigned int idx)
	{
		if (idx >= PyTuple_Size(PO(this->data)))
			throw PythonOutOfBoundsException("Tuple out of bounds");

		PyObject* data = PyTuple_GET_ITEM(PO(this->data), idx);
		Object ret(data, new TupleSetter(*this, idx));
		checkError();
		return ret;
	}
}
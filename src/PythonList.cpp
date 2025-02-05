#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonList.h"
#include "pythoncpp/PythonException.h"

namespace Python
{
	// List iterator

	List::Iterator::Iterator(List& list)
		: list(list)
		, idx(0)
	{
		this->cache();
	}

	List::Iterator::Iterator(const Iterator& it)
		: list(it.list)
		, idx(it.idx)
		, val(it.val)
	{
	}

	List::Iterator List::Iterator::end(List& list)
	{
		List::Iterator it(list);
		it.idx = list.getSize();
		return it;
	}

	void List::Iterator::cache()
	{
		if (PyList_Size(PO(this->list.data)) > 0)
			this->val = Object(PyList_GetItem(PO(this->list.data), this->idx));

		checkError();
	}

	List::Iterator& List::Iterator::operator++()
	{
		this->idx++;
		return *this;
	}

	List::Iterator List::Iterator::operator++(int)
	{
		List::Iterator it = *this;
		this->operator++();
		return it;
	}

	bool List::Iterator::operator==(const Iterator& it)
	{
		return
			this->list == it.list &&
			this->idx == it.idx;
	}

	bool List::Iterator::operator!=(const Iterator& it)
	{
		return !(*this == it);
	}

	Object List::Iterator::operator*()
	{
		this->cache();
		return this->val;
	}

	Object* List::Iterator::operator->()
	{
		this->cache();
		return &this->val;
	}

	// Setter

	class ListSetter : public Setter
	{
	public:
		ListSetter(Object& container, unsigned int key)
			: container(container)
			, key(key)
		{
		}

		void set(const Object& data) override
		{
			PyList_SetItem(PO(this->container), this->key, PO(data));
			checkError();
		}

	private:
		Object& container;
		unsigned int key;
	};

	// List

	List::List()
		: Object(PyList_New(0))
	{
		checkError();
	}

	List::List(pyobj data)
		: Object(data)
	{
	}

	List::List(std::vector<Object> data)
		: Object(PyList_New(data.size()))
	{
		checkError();
		unsigned int i = 0;
		for (Object d : data)
		{
			PyList_SetItem(PO(this->data), i++, PO(d));
			checkError();
		}
	}

	List::List(const List& data)
		: Object(deepcopy(PO(data.data)))
	{
	}

	List::List(Object&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	List::List(List&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	List& List::operator=(Object&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
		return *this;
	}

	List& List::operator=(List&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
		return *this;
	}

	size_t List::getSize() const
	{
		size_t ret = PyList_Size(PO(this->data));
		checkError();
		return ret;
	}

	const Object List::operator[](unsigned int idx) const
	{
		if (idx >= this->getSize())
			throw PythonOutOfBoundsException(std::string("List out of bounds, requested index ") + std::to_string(idx) + ", size " + std::to_string(this->getSize()));
		checkError();

		Object ret = Object(PyList_GetItem(PO(this->data), idx));
		checkError();
		return ret;
	}

	Object List::operator[](unsigned int idx)
	{
		if (idx >= this->getSize())
			throw PythonOutOfBoundsException(std::string("List out of bounds, requested index ") + std::to_string(idx) + ", size " + std::to_string(this->getSize()));
		checkError();

		Object ret = Object(PyList_GetItem(PO(this->data), idx), new ListSetter(*this, idx));
		checkError();
		return ret;
	}

	void List::pushItem(Object item)
	{
		PyList_Append(PO(this->data), PO(item.data));
		checkError();
	}

	List::Iterator List::begin()
	{
		return Iterator(*this);
	}

	List::Iterator List::end()
	{
		return Iterator::end(*this);
	}
}
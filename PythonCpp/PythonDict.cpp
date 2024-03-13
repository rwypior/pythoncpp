#include "private/Python.hpp"
#include "PythonDict.h"
#include "PythonModule.h"
#include "PythonException.h"

namespace Python
{
	// Dictionary iterator

	Dict::Iterator::Iterator(const Dict& dict, int ret)
		: dict(dict)
		, pos(0)
		, ret(ret)
	{
	}

	Dict::Iterator::Iterator(const Dict& dict)
		: dict(dict)
		, pos(0)
	{
		this->next();
	}

	Dict::Iterator::Iterator(const Iterator& it)
		: dict(it.dict)
		, ret(it.ret)
		, pos(it.pos)
		, key(it.key)
		, val(it.val)
	{
	}

	Dict::Iterator::~Iterator()
	{
	}

	Dict::Iterator Dict::Iterator::end(const Dict& dict)
	{
		return Dict::Iterator(dict, 0);
	}

	void Dict::Iterator::next()
	{
		this->ret = PyDict_Next(PO(dict.data), reinterpret_cast<Py_ssize_t*>(&this->pos), reinterpret_cast<PyObject**>(&this->key), reinterpret_cast<PyObject**>(&this->val.data));
		Py_INCREF(this->val.data);
		checkError();
	}

	Dict::Iterator& Dict::Iterator::operator++()
	{
		this->next();
		return *this;
	}

	Dict::Iterator& Dict::Iterator::operator++(int)
	{
		Dict::Iterator& it = *this;
		this->operator++();
		return it;
	}

	bool Dict::Iterator::operator==(const Iterator& it)
	{
		return
			this->dict == it.dict &&
			this->ret == it.ret;
	}

	bool Dict::Iterator::operator!=(const Iterator& it)
	{
		return !(*this == it);
	}

	Object& Dict::Iterator::operator*()
	{
		return this->val;
	}

	Object* Dict::Iterator::operator->()
	{
		return &this->val;
	}

	std::string Dict::Iterator::getKey() const
	{
		return pyobj_to_string(this->key);
	}

	// Setter

	class DictSetter : public Setter
	{
	public:
		DictSetter(Object& container, const Object& key)
			: container(container)
			, key(key)
		{
		}

		void set(const Object& data) override
		{
			PyDict_SetItem(PO(this->container), PO(this->key), PO(data));
			checkError();
		}

	private:
		Object& container;
		const Object& key;
	};

	// Dictionary

	Dict::Dict()
		: Object(PyDict_New())
	{
		checkError();
	}

	Dict::Dict(pyobj data)
		: Object(data)
	{ }

	Dict::Dict(const Dict& data)
		: Object(deepcopy(PO(data.data)))
	{
		if (this->data)
			Py_INCREF(this->data);
	}

	Dict::Dict(Object&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	Dict::Dict(Dict&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
	}

	Dict::Dict(const std::unordered_map<std::string, Object>& data)
		: Dict()
	{
		for (const auto& pair : data)
		{
			(*this)[pair.first] = pair.second;
		}
	}

	Dict& Dict::operator=(Object&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
		return *this;
	}

	Dict& Dict::operator=(Dict&& data) noexcept
	{
		this->data = std::move(data.data);
		data.data = nullptr;
		return *this;
	}

	void Dict::append(const std::string& key, Object& data)
	{
		PyMapping_SetItemString(PO(this->data), key.c_str(), PO(data.data));
		checkError();
	}

	void Dict::append(const std::string& key, const Object& data)
	{
		PyMapping_SetItemString(PO(this->data), key.c_str(), PO(data.data));
		checkError();
	}

	void Dict::append(const std::string& key, Object&& data)
	{
		PyMapping_SetItemString(PO(this->data), key.c_str(), PO(data.data));
		checkError();
	}

	void Dict::append(const std::string& key, Module& module)
	{
		PyMapping_SetItemString(PO(this->data), key.c_str(), PO(module.module));
		checkError();
	}

	bool Dict::contains(const Object& key)
	{
		bool ret = PyDict_Contains(PO(this->data), PO(key.data));
		checkError();
		return ret;
	}

	Object Dict::operator[](const Object& key)
	{
		if (!PyDict_Contains(PO(this->data), PO(key)))
			PyDict_SetItem(PO(this->data), PO(key), PO(Object::none()));
		checkError();
		PyObject* data = PyDict_GetItem(PO(this->data), PO(key));
		checkError();

		Object ret(data, new DictSetter(*this, key));
		checkError();
		return ret;
	}

	const Object Dict::operator[](const Object& key) const
	{
		if (!PyDict_Contains(PO(this->data), PO(key)))
			throw PythonOutOfBoundsException(std::string("Dictionary doesn't contain element ") + key.toString());
		checkError();
		PyObject* data = PyDict_GetItem(PO(this->data), PO(key));
		checkError();

		Object ret(data);
		checkError();
		return ret;
	}

	Object Dict::at(const Object& key) const
	{
		if (!PyDict_Contains(PO(this->data), PO(key)))
			throw PythonOutOfBoundsException("Key \"" + key.toString() + "\" doesn't exist");
		Object ret(PyDict_GetItem(PO(this->data), PO(key)));
		checkError();
		return ret;
	}

	size_t Dict::getSize() const
	{
		size_t ret = PyDict_Size(PO(this->data));
		checkError();
		return ret;
	}

	Dict::Iterator Dict::begin()
	{
		return Iterator(*this);
	}

	Dict::Iterator Dict::end()
	{
		return Iterator::end(*this);
	}

	Dict::Iterator Dict::begin() const
	{
		return Iterator(*this);
	}

	Dict::Iterator Dict::end() const
	{
		return Iterator::end(*this);
	}
}
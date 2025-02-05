#include "PythonCppCommon.hpp"
#include "pythoncpp/PythonObject.h"

namespace Python
{
	// Raw objects

	size_t getReferenceCount(pyobj obj)
	{
		return Py_REFCNT(obj);
	}

	void decrementReferenceCount(pyobj obj)
	{
		Py_DECREF(obj);
	}

	void incrementReferenceCount(pyobj obj)
	{
		Py_INCREF(obj);
	}

	// Setter

	void Setter::set(const std::string& data)
	{
		PyObject *obj = PyUnicode_FromString(data.c_str());
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(int data)
	{
		PyObject* obj = PyLong_FromLong(data);
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(unsigned int data)
	{
		PyObject* obj = PyLong_FromUnsignedLong(data);
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(long data)
	{
		PyObject* obj = PyLong_FromLong(data);
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(unsigned long data)
	{
		PyObject* obj = PyLong_FromUnsignedLong(data);
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(float data)
	{
		PyObject* obj = PyFloat_FromDouble(data);
		checkError();
		this->set(Object(obj));
	}

	void Setter::set(double data)
	{
		PyObject* obj = PyFloat_FromDouble(data);
		checkError();
		this->set(Object(obj));
	}

	// Default setter

	class DefaultDataSetter : public Setter
	{
	public:
		DefaultDataSetter(Object &data)
			: data(data)
		{ }

		virtual void set(const Object& data) override
		{
			if (this->data)
				Py_DECREF(PO(this->data));
			this->data.data = data.data;
		}

	private:
		Object& data;
	};

	// Default synchronizer

	class DefaultSynchronizer : public Synchronizer
	{
	public:
		virtual void synchronize(pyobj& /* obj */) override { }
	};

	// Object

	Object::Object()
		: data(nullptr)
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
	}

	Object::Object(const Object& data)
		: data(deepcopy(PO(data.data)))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		// Returning object may still be the same for primitive types
		// See more in 'deepcopy' function
		assert(!this->data || Py_REFCNT(this->data) >= 1);
	}

	Object::Object(Object&& data) noexcept
		: data(std::move(data.data))
		, setter(std::move(data.setter))
		, synchronizer(std::move(data.synchronizer))
	{
		data.data = nullptr;
	}

	Object::Object(pyobj data)
		: data(data)
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		if (data)
			Py_INCREF(data);
	}

	Object::Object(pyobj data, Setter *setter, Synchronizer *synchronizer)
		: data(data)
		, setter(setter)
		, synchronizer(synchronizer ? synchronizer : new DefaultSynchronizer())
	{
		if (data)
			Py_INCREF(data);
	}

	Object::Object(const std::string& data)
		: data(PyUnicode_FromString(data.c_str()))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(const char* data)
		: Object(std::string(data))
	{
	}

	Object::Object(int data)
		: data(PyLong_FromLong(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(unsigned int data)
		: data(PyLong_FromUnsignedLong(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(long data)
		: data(PyLong_FromLong(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(unsigned long data)
		: data(PyLong_FromUnsignedLong(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(float data)
		: data(PyFloat_FromDouble(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object::Object(double data)
		: data(PyFloat_FromDouble(data))
		, setter(new DefaultDataSetter(*this))
		, synchronizer(new DefaultSynchronizer())
	{
		assert(Py_REFCNT(this->data) >= 1);
		checkError();
	}

	Object& Object::operator=(const Object& data)
	{
		if (data.data)
			Py_INCREF(data.data);
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(const std::string& data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(const char* data)
	{
		return (*this = std::string(data));
	}

	Object& Object::operator=(int data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(unsigned int data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(long data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(unsigned long data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(float data)
	{
		this->setter->set(data);
		return *this;
	}

	Object& Object::operator=(double data)
	{
		this->setter->set(data);
		return *this;
	}

	Object::~Object()
	{
		if (this->data && !this->isNone())
			Py_DECREF(this->data);
	}

	pyobj Object::getType() const
	{
		pyobj ret = PyObject_Type(PO(this->data));
		checkError();
		return ret;
	}

	Object::operator pyobj() const
	{
		// TODO - try to remove this incref
		if (this->data)
			Py_INCREF(this->data);
		return this->data;
	}

	std::string Object::toString() const
	{
		this->synchronizer->synchronize(this->data);
		std::string s = pyobj_to_string(this->data);
		checkError();
		return s;
	}

	long Object::toLong() const
	{
		this->synchronizer->synchronize(this->data);
		long ret = PyLong_AsLong(PO(this->data));
		checkError();
		return ret;
	}

	double Object::toDouble() const
	{
		this->synchronizer->synchronize(this->data);
		double ret = PyFloat_AsDouble(PO(this->data));
		checkError();
		return ret;
	}

	bool Object::toBool() const
	{
		this->synchronizer->synchronize(this->data);
		bool ret = this->data && this->toLong();
		checkError();
		return ret;
	}

	std::string Object::repr() const
	{
		this->synchronizer->synchronize(this->data);
		return pyobj_repr(this->data);
	}

	bool Object::isNone() const
	{
		this->synchronizer->synchronize(this->data);
		return this->data == Py_None;
	}

	bool Object::isNull() const
	{
		return !this->data;
	}

	unsigned int Object::getReferenceCount() const
	{
		return static_cast<unsigned int>(Py_REFCNT(this->data));
	}

	Object Object::none()
	{
		Object d;
		d.data = Py_None;
		return d;
	}
}
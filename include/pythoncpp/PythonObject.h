#ifndef _h_python_data
#define _h_python_data

#include "pythoncpp/PythonDefs.h"

#include <string>
#include <memory>

namespace Python
{
	class Object;

	size_t getReferenceCount(pyobj obj);
	void decrementReferenceCount(pyobj obj);
	void incrementReferenceCount(pyobj obj);

	class Setter
	{
	public:
		virtual ~Setter() = default;

		virtual void set(const Object& data) = 0;

		virtual void set(const std::string& data);
		virtual void set(int data);
		virtual void set(unsigned int data);
		virtual void set(long data);
		virtual void set(unsigned long data);
		virtual void set(float data);
		virtual void set(double data);
	};

	class Synchronizer
	{
	public:
		virtual ~Synchronizer() = default;

		virtual void synchronize(pyobj& obj) = 0;
	};

	class Object
	{
		friend class Dict;
		friend class Tuple;
		friend class List;
		friend class Function;
		friend class Instance;
		friend class Module;
		friend class RefData;
		friend class DefaultDataSetter;

	public:
		Object();
		Object(const Object& data);
		Object(Object&& data) noexcept;

		explicit Object(pyobj data);
		Object(pyobj data, Setter *setter, Synchronizer *synchronizer = nullptr);

		Object(const std::string& data);
		Object(const char *data);
		Object(int data);
		Object(unsigned int data);
		Object(long data);
		Object(unsigned long data);
		Object(float data);
		Object(double data);

		virtual Object& operator=(const Object& data);
		virtual Object& operator=(const std::string& data);
		virtual Object& operator=(const char *data);
		virtual Object& operator=(int data);
		virtual Object& operator=(unsigned int data);
		virtual Object& operator=(long data);
		virtual Object& operator=(unsigned long data);
		virtual Object& operator=(float data);
		virtual Object& operator=(double data);

		virtual ~Object();

		pyobj getType() const;

		/**
		* Get raw pointer to Python object.
		* This call increments the reference counter. The caller is responsible for
		* managing the raw pointer's reference count.
		*/
		[[nodiscard]] operator pyobj() const;

		std::string toString() const;
		std::string repr() const;
		long toLong() const;
		double toDouble() const;
		bool toBool() const;
		bool isNone() const;
		bool isNull() const;

		unsigned int getReferenceCount() const;

		static Object none();

	protected:
		mutable pyobj data;
		std::unique_ptr<Setter> setter = nullptr;
		std::unique_ptr<Synchronizer> synchronizer = nullptr;
	};
}

#endif
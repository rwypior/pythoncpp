#ifndef _h_python_function
#define _h_python_function

#include "PythonDefs.h"
#include "PythonObject.h"

#include <vector>
#include <functional>

namespace Python
{
	class Tuple;

	template<typename T>
	class Ref : public T
	{
	public:
		Ref(const Ref<T>& r) = delete;
	};

	template<typename T, 
		std::enable_if_t<std::is_class<typename std::remove_reference<T>::type>::value, bool> = true, 
		std::enable_if_t<std::is_base_of<Object, typename std::remove_reference<T>::type>::value, bool> = true
	>
	pyobj makeref(T&& t) {
		return static_cast<pyobj>(t);
	};

	template<typename T, 
		std::enable_if_t<std::is_class<typename std::remove_reference<T>::type>::value, bool> = true, 
		std::enable_if_t<!std::is_base_of<Object, typename std::remove_reference<T>::type>::value, bool> = true
	>
	pyobj makeref(T&& t) {
		return static_cast<pyobj>(t);
	};

	template<typename T, 
		std::enable_if_t<!std::is_class<typename std::remove_reference<T>::type>::value, bool> = true
	>
	pyobj makeref(T&& t) {
		return static_cast<pyobj>(Object(t));
	};

	class Function
	{
		friend class Module;
		friend class Inspect;

	public:
		Function(pyobj function);
		~Function();

		Object call();
		Object call(const Tuple &params);
		Object call(std::vector<Object> params);
		Object call_v(std::vector<Object> params);
		Object callref(std::vector<std::reference_wrapper<Object>> params);

		template<typename ...T>
		Object operator()(T&&... params)
		{
			Tuple tuple(std::vector<pyobj> { makeref(params)... });
			return this->call(tuple);
		}

	protected:
		pyobj function;
	};
}

#endif
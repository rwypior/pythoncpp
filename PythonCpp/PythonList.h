#ifndef _h_python_list
#define _h_python_list

#include "PythonDefs.h"
#include "PythonObject.h"

#include <iterator>
#include <vector>

namespace Python
{
	class List : public Object
	{
	public:
		class Iterator
		{
			friend class List;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Object;
			using difference_type = int;
			using pointer = Object*;
			using reference = Object&;

		public:
			Iterator(List& dict);
			Iterator(const Iterator& it);
			Iterator& operator++();
			Iterator operator++(int);
			bool operator==(const Iterator& it);
			bool operator!=(const Iterator& it);
			Object operator*();
			Object* operator->();

		private:
			static Iterator end(List& dict);
			void cache();

			List& list;
			size_t idx;
			Object val;
		};

	public:
		List();
		explicit List(pyobj data);
		List(std::vector<Object> data);

		List(const List& data);

		List(Object&& data) noexcept;
		List(List&& data) noexcept;

		List& operator=(Object&& data) noexcept;
		List& operator=(List&& data) noexcept;

		size_t getSize() const;

		const Object operator[](unsigned int idx) const;
		Object operator[](unsigned int idx);

		void pushItem(Object item);

		Iterator begin();
		Iterator end();
	};
}

#endif
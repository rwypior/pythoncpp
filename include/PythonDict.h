#ifndef _h_python_dict
#define _h_python_dict

#include "PythonDefs.h"
#include "PythonObject.h"

#include <iterator>
#include <string>
#include <unordered_map>

namespace Python
{
	class Dict : public Object
	{
	public:
		class Iterator
		{
			friend class Dict;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Object;
			using difference_type = int;
			using pointer = Object*;
			using reference = Object&;

		public:
			Iterator(const Dict& dict);
			Iterator(const Iterator& it);
			~Iterator();
			Iterator& operator++();
			Iterator& operator++(int);
			bool operator==(const Iterator& it);
			bool operator!=(const Iterator& it);
			Object& operator*();
			Object* operator->();

			std::string getKey() const;

		private:
			Iterator(const Dict& dict, int ret);
			static Iterator end(const Dict& dict);

			void next();

			const Dict& dict;
			int ret = 1;
			size_t pos;
			pyobj* key = nullptr;
			Object val;
		};

	public:
		Dict();
		Dict(pyobj data);
		Dict(const Dict& data);

		Dict(Object&& data) noexcept;
		Dict(Dict&& data) noexcept;

		explicit Dict(const std::unordered_map<std::string, Object> &data);

		Dict& operator=(Object&& data) noexcept;
		Dict& operator=(Dict&& data) noexcept;

		void append(const std::string& key, Object& data);
		void append(const std::string& key, const Object& data);
		void append(const std::string& key, Object&& data);
		void append(const std::string& key, Module& module);

		bool contains(const Object& key);
		Object operator[](const Object& key);
		const Object operator[](const Object& key) const;
		Object at(const Object& key) const;

		size_t getSize() const;

		Iterator begin();
		Iterator end();

		Iterator begin() const;
		Iterator end() const;
	};
}

#endif
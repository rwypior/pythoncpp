#ifndef _h_python_defs
#define _h_python_defs

#include <exception>
#include <string>
#include <vector>
#include <limits>

#define PYTHON_CPP_FLAG_OP(TARGET) \
	inline TARGET operator|(TARGET a, TARGET b) { return static_cast<TARGET>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); } \
	inline TARGET operator&(TARGET a, TARGET b) { return static_cast<TARGET>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b)); } \
	inline TARGET operator|=(TARGET &a, TARGET b) { reinterpret_cast<unsigned int&>(a) |= static_cast<unsigned int>(b); return a; } \
	inline TARGET operator&=(TARGET &a, TARGET b) { reinterpret_cast<unsigned int&>(a) &= static_cast<unsigned int>(b); return a; } \
	inline TARGET operator~(TARGET a) { return static_cast<TARGET>(~static_cast<unsigned int>(a)); } \
	inline bool operator!(TARGET a) { return !static_cast<bool>(a); }

namespace Python
{
	using pyobj = void*;

	class Module;
	class Class;

	enum class FunctionFlag
	{
		VarArgs = 0x01,
		Keywords = 0x02,
		NoArgs = 0x04,
		OneArg = 0x08,
		ClassMethod = 0x10,
		StaticMethod = 0x20,
		Coexist = 0x40,
		Fastcall = 0x80
	};
	PYTHON_CPP_FLAG_OP(FunctionFlag);

	constexpr FunctionFlag DefaultFunctionFlag = FunctionFlag::VarArgs;

	struct PythonFunctionDefContainer
	{ };

	struct PythonClassDefContainer
	{ };

	constexpr size_t objsize();

	namespace Types
	{
		typedef void* (*fncPtr)(void* pySelf, void* pyArgs);
		typedef int (*initFncPtr)(void *self , void *args, void *kwargs);
		typedef void* (*newFncPtr)(void* type, void* args, void* kwargs);
		typedef void* (*allocFncPtr)(void* type, int64_t size);
		typedef void (*deallocFncPtr)(void* self);
		typedef void* (*reprFncPtr)(void* self);
		typedef void* (*setAttrFunc)(void* self, void* args);
		
		struct FunctionDefinition
		{
			std::string name;
			fncPtr function;
			std::string docString;
			FunctionFlag flags;

			FunctionDefinition(const std::string& name, fncPtr function, FunctionFlag flags = FunctionFlag::VarArgs, const std::string& docString = "")
				: name(name)
				, function(function)
				, docString(docString)
				, flags(flags)
			{ }
		};

		struct MemberDefinition
		{
			enum class Type
			{
				Short,
				Int,
				Long,
				Float,
				Double,
				String,
				Object,
				ObjectEx,
				Char,
				Byte,
				Ubyte,
				Uint,
				Ushort,
				Ulong,
				Bool,
				Longlong,
				Ulonglong,
				Pysizet
			};

			std::string name;
			Type type;
			int64_t offset;
			bool readonly = false;
			std::string docstring = "";
		};

		struct ClassDefinition
		{
			std::string name;
			std::string docString;
			std::vector<FunctionDefinition> functionDefinitions;
			std::vector<MemberDefinition> memberDefinitions;
			size_t typeSize = std::numeric_limits<size_t>::max();
			initFncPtr initFunction = nullptr;
			newFncPtr newFunction = nullptr;
			allocFncPtr allocFunction = nullptr;
			deallocFncPtr deallocFunction = nullptr;
			reprFncPtr reprFunction = nullptr;
			setAttrFunc setAttrFunction = nullptr;

			ClassDefinition(const std::string& name)
				: name(name)
			{
			}

			template<typename T>
			ClassDefinition& setType()
			{
				this->typeSize = sizeof(T);
				return *this;
			}

			ClassDefinition& setName(const std::string& name) { this->name = name; return *this; }
			ClassDefinition& setDocString(const std::string& docstring) { this->docString = docstring; return *this; }
			ClassDefinition& addFunction(FunctionDefinition def) { this->functionDefinitions.push_back(def); return *this; }

			ClassDefinition& setInitFunction(initFncPtr def) { this->initFunction = def; return *this; }
			ClassDefinition& useDefaultInitFunction() { return this->setInitFunction(nullptr); }

			ClassDefinition& setNewFunction(newFncPtr def) { this->newFunction = def; return *this; }
			ClassDefinition& useDefaultNewFunction() { return this->setNewFunction(nullptr); }

			ClassDefinition& setDeleteFunction(deallocFncPtr def) { this->deallocFunction = def; return *this; }
			ClassDefinition& setDefaultDeleteFunction() { return this->setDeleteFunction(nullptr); }

			ClassDefinition& setReprFunction(reprFncPtr def) { this->reprFunction = def; return *this; }
			ClassDefinition& setSetAttrFunction(setAttrFunc def) { this->setAttrFunction = def; return *this; }

			ClassDefinition& addMemberDefinition(MemberDefinition def) { this->memberDefinitions.push_back(def); return *this; }
		};
	}
}

#endif

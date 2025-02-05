#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

#include <string>
#include <vector>
#include <sstream>

struct DataStruct
{
	int first = 0;
	int second = 0;
};

inline void* classfunction_simple(void* pySelf, void* pyArgs)
{
	Python::Python& py = Python::Python::getInstance();

	TestSignal::emitSignal("simple");

	return Python::Object::none();
}

inline void* classfunction_params(void* pySelf, void* pyArgs)
{
	Python::Tuple argsTuple(pyArgs);
	if (
		argsTuple.getSize() == 2 &&
		argsTuple[0].toLong() == 42 &&
		argsTuple[1].toLong() == 1337
		)
	{
		TestSignal::emitSignal("params");
	}

	return Python::Object::none();
}

inline void* classfunction_setparams(void* pySelf, void* pyArgs)
{
	DataStruct& str = Python::getDataStructure<DataStruct>(pySelf);

	Python::Tuple argsTuple(pyArgs);
	if (argsTuple.getSize() == 2)
	{
		str.first = argsTuple[0].toLong();
		str.second = argsTuple[1].toLong();
	}

	return Python::Object::none();
}

inline void* classfunction_sum(void* pySelf, void* pyArgs)
{
	DataStruct& str = Python::getDataStructure<DataStruct>(pySelf);
	return Python::Object(str.first + str.second);
}

inline void* classfunction_static(void* pySelf, void* pyArgs)
{
	return Python::Object(1337);
}

static void* Custom_new(void* type, void* args, void* kwds)
{
	DataStruct* self = Python::initializeClass<DataStruct>(type);

	if (self)
	{
		self->first = 12345;
	}

	return Python::baseObject(self);
}

static int classfunction_init(void* self, void* args, void* kwds)
{
	Python::Tuple pyargs(args);
	auto len = pyargs.getSize();

	if (self && len == 1)
	{
		DataStruct& str = Python::getDataStructure<DataStruct>(self);
		str.second = pyargs[0].toLong();
	}

	return 0;
}

static void* classfunction_repr(void* self)
{
	if (self)
	{
		DataStruct& str = Python::getDataStructure<DataStruct>(self);
		std::stringstream ss;
		ss << "[ first=" << str.first << ", second=" << str.second << " ]";
		return Python::Object(ss.str());
	}

	return Python::Object("<class 'TestClass'>");
}

void createClass(Python::Module &module, const char *classname)
{
	Python::TypeBuilder typeBuilder({
		Python::Types::ClassDefinition(classname)
			.setType<DataStruct>()
			.setDocString("Test doc string")
			.setInitFunction(classfunction_init)
			.setNewFunction(Custom_new)
			.setReprFunction(classfunction_repr)
			.addFunction({"classfunction_simple", classfunction_simple, Python::DefaultFunctionFlag, "Simple testing function"})
			.addFunction({"classfunction_params", classfunction_params, Python::DefaultFunctionFlag, "Testing function with parameters"})
			.addFunction({"classfunction_setparams", classfunction_setparams, Python::DefaultFunctionFlag, "Set class parameters"})
			.addFunction({"classfunction_sum", classfunction_sum, Python::DefaultFunctionFlag, "Return sum of class parameters"})
			.addMemberDefinition({"first", Python::Types::MemberDefinition::Type::Int, offsetof(DataStruct, first)})
			.addMemberDefinition({"second", Python::Types::MemberDefinition::Type::Int, offsetof(DataStruct, second)})
		});
	typeBuilder.buildInto(module);
}

TEST_CASE("Create class and call a method", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module createdModule = py.registerModule("createdModule");

	constexpr char classname[] = "TestClass";
	createClass(createdModule, classname);

	Python::Class testclass = createdModule.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE(TestSignal::signalEmittedTimes("simple") == 0);

	testclassinstance.call("classfunction_simple");
	REQUIRE(TestSignal::signalEmittedTimes("simple") == 1);
}

TEST_CASE("Create class and call a method with args", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module createdModule = py.registerModule("createdModuleArgs");

	constexpr char classname[] = "TestClass";
	createClass(createdModule, classname);

	Python::Class testclass = createdModule.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE(TestSignal::signalEmittedTimes("params") == 0);

	testclassinstance.call("classfunction_params", { 42, 1337 });
	REQUIRE(TestSignal::signalEmittedTimes("params") == 1);

	testclassinstance.call("classfunction_params", { 123, 456 });
	REQUIRE(TestSignal::signalEmittedTimes("params") == 0);
}

TEST_CASE("Create class and use data struct", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module createdModule = py.registerModule("createdModuleData");

	Python::Function stringify = createdModule.createFunction("test_stringify", "def test_stringify(cls): return repr(cls)");

	constexpr char classname[] = "TestClass";
	createClass(createdModule, classname);

	Python::Class testclass = createdModule.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance({ 42 });

	DataStruct& datastruct = Python::getDataStructure<DataStruct>(testclassinstance);
	REQUIRE(datastruct.first == 12345);
	REQUIRE(datastruct.second == 42);
	REQUIRE(stringify(testclassinstance).toString() == "[ first=12345, second=42 ]");

	datastruct.first = 111;
	datastruct.second = 222;

	REQUIRE(stringify(testclassinstance).toString() == "[ first=111, second=222 ]");
}

TEST_CASE("Create class and call function using data struct", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module createdModule = py.registerModule("createdModuleDataFunctions");

	constexpr char classname[] = "TestClass";
	createClass(createdModule, classname);

	Python::Class testclass = createdModule.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance();

	DataStruct& datastruct = Python::getDataStructure<DataStruct>(testclassinstance);

	REQUIRE(datastruct.first == 12345);
	REQUIRE(datastruct.second == 0);
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 12345);
	testclassinstance.call("classfunction_setparams", {42, 1337});
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 1379);
	datastruct.first = 1;
	datastruct.second = 2;
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 3);
}

TEST_CASE("Pass created class instance to function", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module testclassesmod = py.loadModule("testclasses");

	constexpr char classname[] = "TestClass";
	createClass(testclassesmod, classname);

	Python::Class testclass = testclassesmod.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance();
	Python::Function classToStr = testclassesmod.getFunction("classToStr");
	Python::Function modifyClass = testclassesmod.getFunction("modifyClass");

	DataStruct& datastruct = Python::getDataStructure<DataStruct>(testclassinstance);

	REQUIRE(classToStr(testclassinstance).toString() == "12345 0");
	datastruct.first = 1;
	datastruct.second = 2;
	REQUIRE(classToStr(testclassinstance).toString() == "1 2");
	modifyClass(testclassinstance);
	REQUIRE(datastruct.first == 10);
	REQUIRE(datastruct.second == 32);
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 42);
}

TEST_CASE("Create custom class instance from Python", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module testclassesmod = py.loadModule("testclasses");

	constexpr char classname[] = "TestClass";
	createClass(testclassesmod, classname);

	Python::Class testclass = testclassesmod.getClass(classname);
	Python::Function createInstance = testclassesmod.getFunction("createInstance");
	Python::Function classToStr = testclassesmod.getFunction("classToStr");
	Python::Function modifyClass = testclassesmod.getFunction("modifyClass");

	Python::Instance testclassinstance = createInstance();

	DataStruct &datastruct = Python::getDataStructure<DataStruct>(testclassinstance);

	REQUIRE(classToStr(testclassinstance).toString() == "123 456");
	REQUIRE(datastruct.first == 123);
	REQUIRE(datastruct.second == 456);
	datastruct.first = 1;
	datastruct.second = 2;
	REQUIRE(classToStr(testclassinstance).toString() == "1 2");
	modifyClass(testclassinstance);
	REQUIRE(datastruct.first == 10);
	REQUIRE(datastruct.second == 32);
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 42);
}

TEST_CASE("Find module", "[creating-classes]")
{
	Python::Python& py = getPython();

	constexpr char modulename[] = "Class_CreatedModule";
	Python::Module createdModule = py.registerModule(modulename);

	constexpr char classname[] = "Class_CreatedClass";
	createClass(createdModule, classname);

	Python::Module foundModule = py.getModule(modulename);
	Python::Class testclass = foundModule.getClass(classname);
	Python::Instance testclassinstance = testclass.createInstance();

	DataStruct& datastruct = Python::getDataStructure<DataStruct>(testclassinstance);

	REQUIRE(datastruct.first == 12345);
	REQUIRE(datastruct.second == 0);
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 12345);
	testclassinstance.call("classfunction_setparams", { 42, 1337 });
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 1379);
	datastruct.first = 1;
	datastruct.second = 2;
	REQUIRE(testclassinstance.call("classfunction_sum").toLong() == 3);
}

TEST_CASE("Created class is picklable", "[creating-classes]")
{
	Python::Python& py = getPython();

	constexpr char modulename[] = "Class_PicklableModule";
	Python::Module createdModule = py.registerModule(modulename);

	constexpr char classname[] = "Class_PicklableClass";
	createClass(createdModule, classname);

	Python::Module testclassesmod = py.loadModule("testclasses");
	Python::Function testpickleFunction = testclassesmod.getFunction("testpickle");

	REQUIRE_NOTHROW( testpickleFunction() );
}

TEST_CASE("Call static class of created class", "[creating-classes]")
{
	Python::Python& py = getPython();

	Python::Module testclassesmod = py.loadModule("testclasses");

	constexpr char classname[] = "Class_WithStatic";

	Python::TypeBuilder typeBuilder({
		Python::Types::ClassDefinition(classname)
			.setType<DataStruct>()
			.addFunction({"classfunction_static", classfunction_static, Python::DefaultFunctionFlag | Python::FunctionFlag::StaticMethod, "Static method"})
		});
	typeBuilder.buildInto(testclassesmod);

	Python::Function teststaticFunction = testclassesmod.getFunction("teststatic");
	REQUIRE(teststaticFunction().toLong() == 1337);

	Python::Function teststatic2Function = testclassesmod.getFunction("teststatic2");
	REQUIRE(teststatic2Function().toLong() == 1337);

	Python::Function teststatic3Function = testclassesmod.getFunction("teststatic3");
	Python::Class cls = testclassesmod.getClass(classname);
	Python::Instance inst = cls.createInstance();
	REQUIRE(teststatic3Function(inst).toLong() == 1337);
}
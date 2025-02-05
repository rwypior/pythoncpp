#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

std::vector<std::string> print_log;

struct UserStruct
{
	int counter = 0;
};

inline void* cppfunc_return42(void* pySelf, void* pyArgs)
{
	return Python::Object(42);
}

inline void* cppfunc_sum(void* pySelf, void* pyArgs)
{
	Python::Tuple args(pyArgs);

	long a = args[0].toLong();
	long b = args[1].toLong();

	return Python::Object(a + b);
}

inline void* cppfunc_print(void* pySelf, void* pyArgs)
{
	Python::Python& py = Python::Python::getInstance();

	Python::Tuple args(pyArgs);
	Python::Object d = args[0];
	std::string str = d.toString();

	print_log.push_back(str);

	return Python::Object::none();
}

inline void* cppfunc_increment_counter(void* pySelf, void* pyArgs)
{
	Python::Python& py = Python::Python::getInstance();
	UserStruct* us = static_cast<UserStruct*>(py.userPtr);
	us->counter++;
	return Python::Object::none();
}

inline void* cppfunc_return_counter(void* pySelf, void* pyArgs)
{
	Python::Python& py = Python::Python::getInstance();
	UserStruct* us = static_cast<UserStruct*>(py.userPtr);
	return Python::Object(us->counter);
}

TEST_CASE("Creating module", "[module]")
{
	print_log.clear();

	Python::Python& py = getPython();
	UserStruct userStruct;

	py.userPtr = &userStruct;

	Python::ModuleBuilder builder({
		{ "cppfunc_return42", cppfunc_return42, Python::DefaultFunctionFlag, "Return 42" },
		{ "cppfunc_sum", cppfunc_sum, Python::DefaultFunctionFlag, "Sum" },
		{ "cppfunc_increment_counter", cppfunc_increment_counter, Python::DefaultFunctionFlag, "Increment the counter" },
		{ "cppfunc_return_counter", cppfunc_return_counter, Python::DefaultFunctionFlag, "Return the counter value" }
		});
	builder.registerPrintFunction(cppfunc_print);
	Python::Module myMod = py.registerModule("cppmodule_1");
	builder.build(myMod);

	Python::Module testcreatemodules = py.loadModule("testcreatemodules");
	Python::Function call_cppfunc_return42 = testcreatemodules.getFunction("call_cppfunc_return42");
	Python::Function call_cppfunc_sum = testcreatemodules.getFunction("call_cppfunc_sum");
	Python::Function call_print_something = testcreatemodules.getFunction("print_something");
	Python::Function call_increment_and_return_counter = testcreatemodules.getFunction("increment_and_return_counter");

	REQUIRE(call_cppfunc_return42().toLong() == 42);
	REQUIRE(call_cppfunc_sum().toLong() == 579);
	REQUIRE(print_log.empty());
	call_print_something();
	REQUIRE(print_log.empty());
	py.redirectOutput(myMod);
	call_print_something();
	REQUIRE(print_log[0] == "something");
	REQUIRE(print_log[1] == "\n");

	REQUIRE(call_increment_and_return_counter().toLong() == 3);
	REQUIRE(call_increment_and_return_counter().toLong() == 6);
}

TEST_CASE("Getting already imported module", "[module]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Module secondarymodule = py.getModule("secondarymodule");

	Python::Function secondary_return_1337 = secondarymodule.getFunction("secondary_return_1337");
	REQUIRE(secondary_return_1337().toLong() == 1337);
}
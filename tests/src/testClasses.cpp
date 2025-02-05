#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

TEST_CASE("Call a class method with no arguments", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Class testclass = testmodule.getClass("TestClass");
	Python::Instance testclassinstance = testclass.createInstance();
	
	REQUIRE(testclassinstance.call("return42").toLong() == 42);
}

TEST_CASE("Call a class method without self argument", "[class]")
{
	using Catch::Matchers::ContainsSubstring;

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Class testclass = testmodule.getClass("TestClass");
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE_THROWS_WITH(testclassinstance.call("noargs"), ContainsSubstring("takes 0 positional arguments but 1 was given"));
	REQUIRE(testclassinstance.call("static").toLong() == 42);
}

TEST_CASE("Call a class method with two arguments", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Class testclass = testmodule.getClass("TestClass");
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE(testclassinstance.call("mul", {42, 100}).toLong() == 4200);
}

TEST_CASE("Passing instance to a function", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_call_return42_from_class = testmodule.getFunction("test_call_return42_from_class");

	Python::Class testclass = testmodule.getClass("TestClass");
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE(test_call_return42_from_class(testclassinstance).toLong() == 42);
}

TEST_CASE("Passing instance to a function with args", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_call_mul_from_class = testmodule.getFunction("test_call_mul_from_class");

	Python::Class testclass = testmodule.getClass("TestClass");
	Python::Instance testclassinstance = testclass.createInstance();

	REQUIRE(test_call_mul_from_class(testclassinstance, 2, 5).toLong() == 10);
}

TEST_CASE("Using existing class instance", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Instance testclass_instance = testmodule.getVariable("testclass_instance");

	REQUIRE(testclass_instance.call("return42").toLong() == 42);
}

TEST_CASE("Using existing class instance with arguments", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_get_membervar = testmodule.getFunction("test_get_membervar");

	Python::Instance testclassconstructor_instance = testmodule.getVariable("testclassconstructor_instance");

	REQUIRE(testclassconstructor_instance.call("getMembervar").toLong() == 1337);
	REQUIRE(test_get_membervar(testclassconstructor_instance).toLong() == 1337);
	testclassconstructor_instance.call("increaseMembervar", { 1 });
	testclassconstructor_instance.call("increaseMembervar", { 1 });
	testclassconstructor_instance.call("increaseMembervar", { 1 });
	REQUIRE(testclassconstructor_instance.call("getMembervar").toLong() == 1340);
	REQUIRE(test_get_membervar(testclassconstructor_instance).toLong() == 1340);
}

TEST_CASE("Using class instance created in function", "[class]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Function test_get_membervar = testmodule.getFunction("test_get_membervar");
	Python::Function test_create_testclassconstructor_instance = testmodule.getFunction("test_create_testclassconstructor_instance");

	Python::Instance testclassconstructor_instance = test_create_testclassconstructor_instance();

	REQUIRE(testclassconstructor_instance.call("getMembervar").toLong() == 123);
	REQUIRE(test_get_membervar(testclassconstructor_instance).toLong() == 123);
	testclassconstructor_instance.call("increaseMembervar", { 10 });
	REQUIRE(testclassconstructor_instance.call("getMembervar").toLong() == 133);
	REQUIRE(test_get_membervar(testclassconstructor_instance).toLong() == 133);
	testclassconstructor_instance.call("increaseMembervar", { 10 });
	REQUIRE(testclassconstructor_instance.call("getMembervar").toLong() == 143);
	REQUIRE(test_get_membervar(testclassconstructor_instance).toLong() == 143);
}
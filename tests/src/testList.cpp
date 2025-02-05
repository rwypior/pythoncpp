#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Simple list creation", "[list]")
{
	Python::Python& py = getPython();

	Python::List list({ 1, 2, 3 });

	REQUIRE(list.getSize() == 3);

	REQUIRE(list[0].toLong() == 1);
	REQUIRE(list[1].toLong() == 2);
	REQUIRE(list[2].toLong() == 3);
}

TEST_CASE("Create list and set in Python", "[list]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_by_name = testmodule.getFunction("test_print_variable_by_name");

	Python::List list({ 1, 2, 3 });

	testmodule.setVariable("test_created_list", list);

	Python::Object result = test_print_variable_by_name.call_v({ "test_created_list" });
	REQUIRE(result.toString() == "[1, 2, 3]");
}

TEST_CASE("Iterating out of bounds", "[list]")
{
	using Catch::Matchers::ContainsSubstring;

	Python::Python& py = getPython();

	Python::List list({ 1, 2, 3 });

	REQUIRE_THROWS_WITH(list[3], ContainsSubstring("List out of bounds"));
}

TEST_CASE("Iterating list", "[list]")
{
	Python::Python& py = getPython();

	Python::List list({ 1, 2, 3, 4, 5 });

	unsigned int i = 1;
	for (const auto& el : list)
	{
		REQUIRE(el.toLong() == i++);
	}
}

TEST_CASE("Accessing Python list elements", "[list]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::List test_list = testmodule.getVariable("test_list");

	REQUIRE(test_list.getSize() == 3);

	REQUIRE(test_list[0].toLong() == 1);
	REQUIRE(test_list[1].toLong() == 2);
	REQUIRE(test_list[2].toLong() == 3);
}

TEST_CASE("Iterating Python list", "[list]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::List test_list = testmodule.getVariable("test_list");

	long i = 1;
	for (const auto& el : test_list)
	{
		REQUIRE(el.toLong() == i++);
	}
}

TEST_CASE("Modifying list items", "[list]")
{
	Python::Python& py = getPython();

	Python::List list({ 1, 2, 3 });

	REQUIRE(list[0].toLong() == 1);
	REQUIRE(list[1].toLong() == 2);
	REQUIRE(list[2].toLong() == 3);

	list[0] = 10;
	list[1] = 20;
	list[2] = 30;

	REQUIRE(list[0].toLong() == 10);
	REQUIRE(list[1].toLong() == 20);
	REQUIRE(list[2].toLong() == 30);
}

TEST_CASE("Modifying list items during iteration", "[list]")
{
	Python::Python& py = getPython();

	// TODO - functionality not implemented yet
}

TEST_CASE("Modifying Python list", "[list]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_by_name = testmodule.getFunction("test_print_variable_by_name");

	Python::List test_list_to_modify = testmodule.getVariable("test_list_to_modify");

	REQUIRE(test_list_to_modify[0].toLong() == 1);
	REQUIRE(test_list_to_modify[1].toLong() == 2);
	REQUIRE(test_list_to_modify[2].toLong() == 3);

	test_list_to_modify[0] = 10;
	test_list_to_modify[1] = 20;
	test_list_to_modify[2] = 30;

	Python::Object result = test_print_variable_by_name.call_v({ "test_list_to_modify" });
	REQUIRE(result.toString() == "[10, 20, 30]");
}

TEST_CASE("Iterating empty list", "[list]")
{
	Python::Python& py = getPython();

	Python::List empty_list = Python::List();

	REQUIRE(empty_list.getSize() == 0);

	unsigned int iterationCount = 0;
	for (auto any : empty_list)
	{
		(void)any;
		iterationCount++;
	}
	
	REQUIRE(iterationCount == 0);
}
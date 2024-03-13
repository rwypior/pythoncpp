#include "testCommon.h"

#include "pythoncpp.h"

#include <catch/catch.hpp>

TEST_CASE("Getting variables", "[variables]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Object test_variable_42 = testmodule.getVariable("test_variable_42");

	REQUIRE(test_variable_42.toLong() == 42);
}

TEST_CASE("Modifying created variable on C++ side", "[variables]")
{
	Python::Python& py = getPython();

	Python::Object var1 = 1337;
	REQUIRE(var1.toLong() == 1337);

	var1 = 42;
	REQUIRE(var1.toLong() == 42);
}

TEST_CASE("Creating variables", "[variables]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_value = testmodule.getFunction("test_print_variable_value");

	Python::Object test_created_variable = Python::Object(1234);
	REQUIRE(test_created_variable.toLong() == 1234);

	test_created_variable = 5678;
	REQUIRE(test_created_variable.toLong() == 5678);
}

TEST_CASE("Passing variables to function", "[variables]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_value = testmodule.getFunction("test_print_variable_value");

	Python::Object test_variable_42 = testmodule.getVariable("test_variable_42");
	Python::Object test_variable_42_result = test_print_variable_value.call_v({ test_variable_42 });
	REQUIRE(test_variable_42.toLong() == 42);
	REQUIRE(test_variable_42_result.toLong() == 42);

	Python::Object test_created_variable = Python::Object(1234);
	Python::Object test_created_variable_result = test_print_variable_value.call_v({ test_created_variable });
	REQUIRE(test_created_variable.toLong() == 1234);
	REQUIRE(test_created_variable_result.toLong() == 1234);

	test_created_variable = 5678;
	Python::Object test_created_variable_result_2 = test_print_variable_value.call_v({ test_created_variable });
	REQUIRE(test_created_variable.toLong() == 5678);
	REQUIRE(test_created_variable_result_2.toLong() == 5678);
}

TEST_CASE("Modifying variables from Python", "[variables]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_change_variable = testmodule.getFunction("test_function_change_variable");

	Python::Object test_variable_changing_2 = testmodule.getVariable("test_variable_changing_2");
	REQUIRE(test_variable_changing_2.toLong() == 100);
	test_function_change_variable("test_variable_changing_2", 200);
	REQUIRE(test_variable_changing_2.toLong() == 200);
	test_variable_changing_2 = testmodule.getVariable("test_variable_changing_2");
	REQUIRE(test_variable_changing_2.toLong() == 200);
}

TEST_CASE("Altering existing variables", "[variables]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_by_name = testmodule.getFunction("test_print_variable_by_name");
	constexpr char varname[] = "test_variable_changing";

	Python::Object test_variable_changing = testmodule.getVariable(varname);

	REQUIRE(test_variable_changing.toLong() == 1);
	REQUIRE(test_print_variable_by_name(varname).toLong() == 1);

	test_variable_changing = 42;

	//REQUIRE(test_variable_changing.toLong() == 42);
	REQUIRE(test_print_variable_by_name(varname).toLong() == 42);
}
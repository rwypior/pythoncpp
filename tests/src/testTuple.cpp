#include "testCommon.h"

#include "pythoncpp.h"

#include <catch2/catch.hpp>

TEST_CASE("Creating tuple", "[tuple]")
{
	Python::Python& py = getPython();

	unsigned int tupleSize = 5;

	Python::Tuple tuple(tupleSize);

	REQUIRE(tuple.getSize() == tupleSize);

	for (unsigned int i = 0; i < tupleSize; i++)
	{
		REQUIRE(tuple[i].isNull());
	}

	for (unsigned int i = 0; i < tupleSize; i++)
	{
		tuple[i] = i;
	}

	for (unsigned int i = 0; i < tupleSize; i++)
	{
		REQUIRE(tuple[i].toLong() == i);
	}
}

TEST_CASE("Using Python tuple", "[tuple]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Tuple test_tuple = testmodule.getVariable("test_tuple");

	REQUIRE(test_tuple[0].toLong() == 111);
	REQUIRE(test_tuple[1].toLong() == 222);
	REQUIRE(test_tuple[2].toLong() == 333);
}

TEST_CASE("Modifying Python tuple", "[tuple]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_by_name = testmodule.getFunction("test_print_variable_by_name");

	Python::Tuple test_tuple_to_modify = testmodule.getVariable("test_tuple_to_modify");

	REQUIRE(test_tuple_to_modify[0].toLong() == 1);
	REQUIRE(test_tuple_to_modify[1].toLong() == 2);
	REQUIRE(test_tuple_to_modify[2].toLong() == 3);

	test_tuple_to_modify[0] = 10;
	test_tuple_to_modify[1] = 20;
	test_tuple_to_modify[2] = 30;

	Python::Object result = test_print_variable_by_name.call_v({ "test_tuple_to_modify" });
	std::string str = result.toString();
	REQUIRE(str == "(10, 20, 30)");
}
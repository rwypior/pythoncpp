#include "testCommon.h"

#include "pythoncpp.h"

#include <catch/catch.hpp>

#include <unordered_set>
#include <unordered_map>

TEST_CASE("Creating dictionary with map", "[dictionary]")
{
	Python::Python& py = getPython();

	Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
	});

	REQUIRE(dict.getSize() == 3);

	dict["a"];

	REQUIRE(dict["a"].toLong() == 123);
	REQUIRE(dict["b"].toLong() == 456);
	REQUIRE(dict["c"].toLong() == 789);
}

TEST_CASE("Creating dictionary by adding elements", "[dictionary]")
{
	Python::Python& py = getPython();

	Python::Dict dict;

	REQUIRE(dict.getSize() == 0);

	dict["a"] = 1;
	dict["b"] = 2;
	dict["c"] = 3;

	REQUIRE(dict["a"].toLong() == 1);
	REQUIRE(dict["b"].toLong() == 2);
	REQUIRE(dict["c"].toLong() == 3);
}

TEST_CASE("Accessing existing element in const dictionary", "[dictionary]")
{
	using Catch::Matchers::Contains;

	Python::Python& py = getPython();

	const Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
		});

	py.checkErrors();

	REQUIRE(dict["a"].toLong() == 123);
	REQUIRE(dict["b"].toLong() == 456);
	REQUIRE(dict["c"].toLong() == 789);
}

TEST_CASE("Accessing not existing element", "[dictionary]")
{
	using Catch::Matchers::Contains;

	Python::Python& py = getPython();

	const Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
	});

	REQUIRE_THROWS_WITH(dict["d"], Contains("Dictionary doesn't contain element"));
}

TEST_CASE("Iterating dictionary", "[dictionary]")
{
	Python::Python& py = getPython();

	const Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
	});

	std::unordered_set<long> allElements;

	for (const auto& el : dict)
	{
		allElements.insert(el.toLong());
	}

	REQUIRE(allElements.find(123) != allElements.end());
	REQUIRE(allElements.find(456) != allElements.end());
	REQUIRE(allElements.find(789) != allElements.end());
}

TEST_CASE("Iterating dictionary with iterators", "[dictionary]")
{
	Python::Python& py = getPython();

	const Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
	});

	std::unordered_map<std::string, long> allElements;

	for (auto it = dict.begin(); it != dict.end(); it++)
	{
		allElements[it.getKey()] = it->toLong();
	}

	REQUIRE(allElements["a"] == 123);
	REQUIRE(allElements["b"] == 456);
	REQUIRE(allElements["c"] == 789);
}

TEST_CASE("Accessing Python dict elements", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Dict test_dict = testmodule.getVariable("test_dict");

	REQUIRE(test_dict.getSize() == 3);

	REQUIRE(test_dict["a"].toLong() == 1);
	REQUIRE(test_dict["b"].toLong() == 2);
	REQUIRE(test_dict["c"].toLong() == 3);
}

TEST_CASE("Iterating Python dict", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Dict test_dict = testmodule.getVariable("test_dict");

	std::unordered_map<std::string, long> allElements;

	for (auto it = test_dict.begin(); it != test_dict.end(); it++)
	{
		allElements[it.getKey()] = it->toLong();
	}

	REQUIRE(allElements["a"] == 1);
	REQUIRE(allElements["b"] == 2);
	REQUIRE(allElements["c"] == 3);
}

TEST_CASE("Iterating Python dictionary of dictionaries", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Dict test_dict_of_dicts = testmodule.getVariable("test_dict_of_dicts");

	std::unordered_set<long> allElements;

	for (auto &el : test_dict_of_dicts)
	{
		Python::Dict& nested = static_cast<Python::Dict&>(el);
		for (auto el2 : nested)
		{
			allElements.insert(el2.toLong());
		}
	}

	std::vector<long> nums {1, 2, 3, 4, 5, 6, 7, 8, 9};
		
	for (long num : nums)
	{
		REQUIRE(allElements.find(num) != allElements.end());
	}
}

TEST_CASE("Modifying dict items", "[dictionary]")
{
	Python::Python& py = getPython();

	Python::Dict dict({
		{ "a", 1 },
		{ "b", 2 },
		{ "c", 3 }
	});

	REQUIRE(dict["a"].toLong() == 1);
	REQUIRE(dict["b"].toLong() == 2);
	REQUIRE(dict["c"].toLong() == 3);

	dict["a"] = 10;
	dict["b"] = 20;
	dict["c"] = 30;

	REQUIRE(dict["a"].toLong() == 10);
	REQUIRE(dict["b"].toLong() == 20);
	REQUIRE(dict["c"].toLong() == 30);
}

TEST_CASE("Modifying dict items from Python", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_change_dict_element = testmodule.getFunction("test_function_change_dict_element");

	Python::Dict test_dict_to_modify_2 = testmodule.getVariable("test_dict_to_modify_2");

	REQUIRE(test_dict_to_modify_2["a"].toLong() == 1);
	REQUIRE(test_dict_to_modify_2["b"].toLong() == 2);
	REQUIRE(test_dict_to_modify_2["c"].toLong() == 3);

	test_function_change_dict_element("test_dict_to_modify_2", "b", 20);

	REQUIRE(test_dict_to_modify_2["a"].toLong() == 1);
	REQUIRE(test_dict_to_modify_2["b"].toLong() == 20);
	REQUIRE(test_dict_to_modify_2["c"].toLong() == 3);
}

TEST_CASE("Modifying dictionary items during iteration", "[dictionary]")
{
	Python::Python& py = getPython();

	Python::Dict dict({
		{ "a", 123 },
		{ "b", 456 },
		{ "c", 789 }
	});

	dict["a"] = 11;
	dict["b"] = 22;
	dict["c"] = 33;

	std::unordered_map<std::string, long> allElements;

	for (auto it = dict.begin(); it != dict.end(); it++)
	{
		allElements[it.getKey()] = it->toLong();
	}

	REQUIRE(allElements["a"] == 11);
	REQUIRE(allElements["b"] == 22);
	REQUIRE(allElements["c"] == 33);
}

TEST_CASE("Modifying existing Python dictionary", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_print_variable_by_name = testmodule.getFunction("test_print_variable_by_name");

	Python::Dict test_dict_to_modify = testmodule.getVariable("test_dict_to_modify");

	REQUIRE(test_dict_to_modify["a"].toLong() == 1);
	REQUIRE(test_dict_to_modify["b"].toLong() == 2);
	REQUIRE(test_dict_to_modify["c"].toLong() == 3);

	test_dict_to_modify["a"] = 10;
	test_dict_to_modify["b"] = 20;
	test_dict_to_modify["c"] = 30;

	Python::Object result = test_print_variable_by_name.call_v({ "test_dict_to_modify" });
	REQUIRE(result.toString() == "{'a': 10, 'b': 20, 'c': 30}");
}

TEST_CASE("Iterating empty Python dictionary", "[dictionary]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Dict empty_dict = Python::Dict();

	REQUIRE(empty_dict.getSize() == 0);

	unsigned int iterationCount = 0;

	for (auto el : empty_dict)
	{
		(void)el;
		iterationCount++;
	}

	REQUIRE(iterationCount == 0);
}
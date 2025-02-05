#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Basic returning function", "[functions]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_return42 = testmodule.getFunction("test_function_return42");
	Python::Object result = test_function_return42.call();

	REQUIRE(result.toLong() == 42);
	REQUIRE(result.toLong() == 42);
	REQUIRE(result.toLong() == 42);
}

TEST_CASE("Single argument function", "[functions]")
{
	long num1 = 5;
	long num2 = 0;
	float num3 = -13.37f;

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_mul_by_2 = testmodule.getFunction("test_function_mul_by_2");

	Python::Object result1 = test_function_mul_by_2.call_v({ num1 });
	REQUIRE(result1.toLong() == num1 * 2);

	Python::Object result2 = test_function_mul_by_2.call_v({ num2 });
	REQUIRE(result2.toLong() == num2 * 2);
	
	Python::Object result3 = test_function_mul_by_2.call_v({ num3 });
	REQUIRE(result3.toDouble() == Catch::Approx(num3 * 2));
}

TEST_CASE("Two argument function", "[functions]")
{
	std::pair<long, long> pair1 {5, 10};
	std::pair<long, float> pair2 {0, 10.0f};
	std::pair<float, float> pair3 {-13.37f, 4.2f};

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_mul_both = testmodule.getFunction("test_function_mul_both");

	Python::Object result1 = test_function_mul_both.call_v({ pair1.first, pair1.second });
	REQUIRE(result1.toLong() == pair1.first * pair1.second);

	Python::Object result2 = test_function_mul_both.call_v({ pair2.first, pair2.second });
	REQUIRE(result2.toDouble() == Catch::Approx(pair2.first * pair2.second));

	Python::Object result3 = test_function_mul_both.call_v({ pair3.first, pair3.second });
	REQUIRE(result3.toDouble() == Catch::Approx(pair3.first * pair3.second));
}

TEST_CASE("Two argument function using call operator", "[functions]")
{
	long num = 1337;
	long num2 = 42;

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_function_mul_both = testmodule.getFunction("test_function_mul_both");

	Python::Object result = test_function_mul_both(num, num2);
	REQUIRE(result.toLong() == num * num2);
}

TEST_CASE("Create function by string", "[functions]")
{
	constexpr char examplePythonFunctionName[] = "test_function_created_by_string";
	constexpr char examplePythonFunction[] = R"(def test_function_created_by_string(): return 12345)";

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");

	Python::Function createdFunction = testmodule.createFunction(examplePythonFunctionName, examplePythonFunction);
	Python::Object result = createdFunction.call();

	REQUIRE(result.toLong() == 12345);
}

TEST_CASE("Calling created function from Python", "[functions]")
{
	constexpr char examplePythonFunctionName[] = "test_function_created_by_string_2";
	constexpr char examplePythonFunction[] = R"(def test_function_created_by_string_2(): return 6789)";

	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Function test_call_function = testmodule.getFunction("test_call_function");

	Python::Function createdFunction = testmodule.createFunction(examplePythonFunctionName, examplePythonFunction);
	(void)createdFunction;

	Python::Object result = test_call_function.call_v({ std::string(examplePythonFunctionName) });

	REQUIRE(result.toString() == (std::string(examplePythonFunctionName) + " result: 6789"));
}
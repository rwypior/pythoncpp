#include "testCommon.h"

#include "pythoncpp.h"

#include <catch/catch.hpp>

TEST_CASE("Basic inspect usage", "[inspect]")
{
	Python::Python& py = getPython();
	Python::Module testmodule = py.loadModule("testmodule");
	Python::Inspect inspect;

	Python::Function test_function_return42 = testmodule.getFunction("test_function_return42");
	Python::Module foundModule = inspect.getModule(test_function_return42);

	std::string moduleName = foundModule.getName();
	REQUIRE(moduleName == "testmodule");
}
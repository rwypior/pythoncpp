#include "testCommon.h"

#include "pythoncpp/PythonCpp.h"

#include <catch2/catch_all.hpp>

TEST_CASE("Refcnt of created object", "[memory]")
{
	Python::Python& py = getPython();

	Python::Object x = "An_unique_value_1";
	REQUIRE(x.getReferenceCount() == 1);
}

TEST_CASE("Refcnt after object destruction", "[memory]")
{
	Python::Python& py = getPython();

	Python::pyobj rawObject;

	{
		Python::Object data = "An_unique_value_2";
		REQUIRE(data.getReferenceCount() == 1);
		rawObject = data;
		REQUIRE(data.getReferenceCount() == 2);
	}

	REQUIRE(Python::getReferenceCount(rawObject) == 1);
}
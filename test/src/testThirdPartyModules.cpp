#include "testCommon.h"

#include "pythoncpp.h"

#include <catch/catch.hpp>

TEST_CASE("Create an object from 3rd-party module", "[!throws]")
{
	Python::Python& py = getPython();
	Python::Module pyglm = py.loadModule("glm");
	Python::Instance vec3a = pyglm.getClass("vec3").createInstance();

	Python::Object type(vec3a.getType());
	REQUIRE(type.toString() == "<class 'glm.vec3'>");
}

TEST_CASE("Get a third-party module loaded by another module", "[!throws]")
{
	Python::Python& py = getPython();
	Python::Module test3rdpartymodule = py.loadModule("test_3rd_party");
	Python::Module pyglm = py.getModule("glm");
	Python::Instance vec3a = pyglm.getClass("vec3").createInstance();

	Python::Object type(vec3a.getType());
	REQUIRE(type.toString() == "<class 'glm.vec3'>");
}

TEST_CASE("Constructing objects from 3rd-party modules", "[!throws]")
{
	Python::Python& py = getPython();
	Python::Module test3rdpartymodule = py.loadModule("test_3rd_party");
	Python::Function printVec3ByName = test3rdpartymodule.getFunction("printVec3ByName");

	Python::Module pyglm = py.loadModule("glm");
	Python::Instance vec3a = pyglm.getClass("vec3").createInstance({5.0, 6.0, 7.0});

	test3rdpartymodule.setVariable("vec3_created", vec3a);

	Python::Object res = printVec3ByName.call_v({"vec3_created"});
	REQUIRE(res.toString() == "x5.0 y6.0 z7.0");
}
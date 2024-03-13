import glm

def printVec3ByName(varname):
	vec = globals()[varname]
	return f"x{vec.x} y{vec.y} z{vec.z}"

test_vec3_empty = glm.vec3()
test_vec3 = glm.vec3(1.0, 5.0, 10.0)
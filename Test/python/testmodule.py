import secondarymodule

def test_function_return42():
	return 42

def test_function_mul_by_2(num):
	return num * 2

def test_function_mul_both(a, b):
	return a * b

def test_call_function(func):
	result = globals()[func]()
	return f"{func} result: {result}"

def test_print_variable_by_name(varname):
	result = globals()[varname]
	return result

def test_print_variable_value(value):
	return value

def test_function_change_variable(name, newValue):
	globals()[name] = newValue

def test_function_change_dict_element(name, index, newValue):
	globals()[name][index] = newValue

def test_call_return42_from_class(obj):
	return obj.return42()

def test_call_mul_from_class(obj, a, b):
	return obj.mul(a, b)

def test_get_membervar(obj):
	return obj.getMembervar()

class TestClass:
	def return42(self):
		return 42

	def mul(self, a, b):
		return a * b

	def noargs():
		return 1

	@staticmethod
	def static():
		return 42

class TestClassConstructor:
	def __init__(self, param):
		self.membervar = param

	def getMembervar(self):
		return self.membervar

	def increaseMembervar(self, num):
		self.membervar += num

def test_create_testclassconstructor_instance():
	return TestClassConstructor(123)

test_variable_42 = 42
test_variable_changing = 1
test_variable_changing_2 = 100

test_tuple = (111, 222, 333)
test_tuple_to_modify = (1, 2, 3)

test_list = [1, 2, 3]
test_list_to_modify = [1, 2, 3]

test_dict = {
	"a": 1,
	"b": 2,
	"c": 3
}
test_dict_to_modify = {
	"a": 1,
	"b": 2,
	"c": 3
}
test_dict_to_modify_2 = {
	"a": 1,
	"b": 2,
	"c": 3
}
test_dict_of_dicts = {
	"first": {
		"a": 1,
		"b": 2,
		"c": 3
	},
	"second": {
		"a": 4,
		"b": 5,
		"c": 6
	},
	"third": {
		"a": 7,
		"b": 8,
		"c": 9
	}
}

testclass_instance = TestClass()
testclassconstructor_instance = TestClassConstructor(1337)
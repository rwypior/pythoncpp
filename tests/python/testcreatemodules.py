import cppmodule_1

def call_cppfunc_return42():
	return cppmodule_1.cppfunc_return42()

def call_cppfunc_sum():
	return cppmodule_1.cppfunc_sum(123, 456)

def print_something():
	print("something")

def increment_and_return_counter():
	cppmodule_1.cppfunc_increment_counter()
	cppmodule_1.cppfunc_increment_counter()
	cppmodule_1.cppfunc_increment_counter()
	return cppmodule_1.cppfunc_return_counter()
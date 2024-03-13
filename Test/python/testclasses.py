import pickle

def classToStr(obj):
	return f"{obj.first} {obj.second}"

def modifyClass(obj):
	obj.first = 10
	obj.second = 32

def createInstance():
	tc = TestClass()
	tc.first = 123
	tc.second = 456
	return tc

def testpickle():
	import Class_PicklableModule
	c = Class_PicklableModule.Class_PicklableClass()
	pickle.dumps(c)

def teststatic():
	c = Class_WithStatic()
	return type(c).classfunction_static()

def teststatic2():
	return Class_WithStatic.classfunction_static()

def teststatic3(inst):
	return type(inst).classfunction_static()
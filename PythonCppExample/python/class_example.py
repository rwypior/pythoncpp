# Import 'cppModule' module created from C++
# The 'cppModule' module must be created before importing this module
import cppModule

def visit_cpp_class(theClass):
    counterValue = theClass.spinCounter()
    print(f"New counter value is {counterValue}")
# Import 'cppModule' module created from C++
# The 'cppModule' module must be created before importing this module
import cppModule

# Simple function which calls a function from 'cppModule'
def call_cpp_func():
    result = cppModule.cppFunc("Hi from Python!")
    print(f"cppModule responded with {result}!")
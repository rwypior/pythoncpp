"""This is an example Python module that will be used from the C++ side"""

def basic_sum(a, b):
    """This is simple example function that sums two numbers"""
    return a + b
    
class example_class():
    """This is an example Python class"""
    
    def __init__(self):
        print("Creating instance")
        self.do_bad_stuff()
        
    def hello(self, a, b):
        print("Hello from instance!")
        return a + b
# PythonCpp

An utility library encapsulating the functionality of Python's C-API in C++.

## Usage

The library may be used as static library, or by embedding the source files directly into target project.
In order to start using PythonCpp in C++ project, it's required to create an instance of PythonCpp in one of two ways:
Using utility singleton-like function getInstance:
```
Python::Python& py = Python::Python::getInstance();
```
Or by instantiating the class the regular way:
```
Python::Python py;
```
> Note that two methods presented above should **not** be used together

This initializes the Python functionality and enables you to create variables, functions, classes, load Python modules and more. In order to load custom modules from .py files, Python path first needs to be extended so that Python knows where to look for the modules:

```
std::string path = std::filesystem::current_path().string() + "\\myscripts";
py.appendPath(path);
```

After executing this code it's possible to load custom modules located in directory specified in `path` variable:

```
Python::Module module = py.loadModule("my_module");
```

Once a module is loaded it is possible to gain access to functions, variables and other members of the module as well as extending the module with custom classes. For more information refer to examples and tests.

## Providing custom Python environment
Often it's required to distribute certain Python environment to make sure the target machine can execute Python interpreter the same way as on the development machine. In order to do so it's required to copy Python's DLLs and Lib directories, and specify both `PYTHONHOME` variable to point to a location in which DLLs and Lib directories are stored, for example, if the project's structure is as follows:
```
C:
|-- MyProject
  |
  |--Python
    |--DLLs
    |--Lib
```
The `PYTHONHOME` should point to `C:\MyProject\Python` directory. In order to ensure correct functionality of Python, it's also required to add the same directory to `PYTHONPATH` variable.
The easiest way of doing so in PythonCpp is to call the following **before** creating any instance of Python class:
```
std::string pythonhome = "C:\MyProject\Python";
Python::Initializer::getDefaultInitializer().setHome(pythonhome);
```
And setting `PYTHONPATH` at any time before loading the modules:
```
py.setPath(pythonhome);
```

## Examples
Example provide a great way to learn new things. That's why few examples are provided at [this location](PythonCppExample).

## Python documentation
An extensive documentation of Python C-API is available at https://docs.python.org/3/c-api/index.html.

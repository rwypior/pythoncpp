# PythonCpp

An utility library encapsulating the functionality of Python's C-API in C++.

## Installation

Clone the repository and use CMake to build and install it:
```
https://github.com/rwypior/pythoncpp.git
cd pythoncpp
mkdir build && cd build
cmake ..
sudo cmake --install .
```

Once the library is installed, link to it in your CMake project:

```
find_package(PythonCpp CONFIG REQUIRED)
target_link_libraries(YOUR_PROJECT PRIVATE PythonCpp)
```

Optionally, the following CMake code may be added to your project in order to locally install your Python copy to your project:
```
include(${PYCPP_INSTALL_PYTHON})
InstallPython()
```
By default InstallPython will install Python to your project's binary tree to `dist/lib/PythonXX` directory. The InstallPython function provides more configuration options which are listed below.

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

## InstallPython()
InstallPython is CMake function provided by this library which allows users to easily copy system's Python installation to your project. If the function is called without any parameters, Python will be installed to your project's binary directory to `dist/lib/PythonXX` directory. Following configuration options are supported:

### DRYRUN
Will cause the script to not perform any actions, only print the operations. Example: `InstallPython(DRYRUN ...)`.

### VERSION /[version/]
Specifies Python version to find. The argument will be directly passed to `find_package(PYTHON ...)`. Example: `InstallPython(VERSION 3.9.1 ...)`.

### SCRIPTPATH /[path/]
Overrides the location of `installpython.py` script which is used internally by this function.

### PACKAGEDIR /[path/]
Specifies directory where Python packages will be installed. By default, the path will be in `site-packages` directory in Python installation directory.

### INSTALLDIR /[path/]
Specifies directory where Python shared libraries will be installed. By default, the path will be in project's binary directory.

### LIBDIR /[path/]
Specifies directory where Python's built-in modules will be installed. By default, the path will be in `dist/lib` directory in Python's installation directory. `PythonXX` directory will be added to the path, where XX is Python's version.

### TMP /[path/]
Specifies path to where store temporary Python wheel files, used for installing Python packages.

### HEADER /[path/]
Specifies path where to generate C++ header file with relative paths to Python's directories. This file may be used for `Initializer::setHome` and `Python::setPath/appendPath` functions to quickly set up the environment. If not specifies - the file will not be generated.

### PACKAGES /[packages/]
A list of Python packages to install. Example - `InstallPython(PACKAGES "pyglm==2.8.0" "another-package")`

All of the above are optional.

## Examples
Example provide a great way to learn new things. That's why few examples are provided at [this location](PythonCppExample).

## Python documentation
An extensive documentation of Python C-API is available at https://docs.python.org/3/c-api/index.html.

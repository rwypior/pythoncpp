#ifndef _h_python_main
#define _h_python_main

#include <vector>
#include <string>
#include <unordered_map>

namespace Python
{
	class Module;
	class List;

	static constexpr char PythonHome[] = "PYTHONHOME";

	// Initializer class provides Python parameters before Python startup.
	// Typical use cases would modify defaultInitializer which is used by default by
	// Python class constructor.
	class Initializer
	{
	public:
		static Initializer& getDefaultInitializer();

		// Change value of PYTHONHOME
		// Typically this is only used when changing the Python interpreter location
		// for an applications which embed Python and Python interpreter
		// This should point to a folder which contains Lib and DLLs directories of Python
		// Typically when this is changed, PYTHONPATH should also contain the same location
		Initializer& setHome(const std::string &home);
		std::string getHome() const;

	private:
		std::unordered_map<std::string, std::string> envs;

		static Initializer defaultInitializer;
	};

	class Python
	{
	public:
		// Utility function used to obtain global Python instance
		// getInstance should not be used with together with regular construction of this class
		// otherwise unexpected behavior may occur
		static Python& getInstance();

		Python(const Initializer& initializer = Initializer::getDefaultInitializer());
		~Python();

		void setArgs(std::vector<std::wstring> argv);
		void setArgs(std::vector<std::string> argv);

		List getPath() const;
		void setPath(const List &list);
		void setPath(const std::string &path);
		void appendPath(const std::wstring& path);
		void appendPath(const std::string& path);

		std::string getHome() const;
		void setHome(const std::wstring &path) const;
		void setHome(const std::string& path) const;

		Module loadModule(const std::string& path);
		Module reloadModule(Module &module);
		Module registerModule(const std::string& name);
		Module getModule(const std::string& name);

		// Redirect Python stream 'streamName' to module provided in 'module'
		// The module which will receive the output should have a function named 'write'
		// which takes a single argument which receives all the output for this stream
		void redirectOutput(const Module& module, const std::string& streamName = "stdout");

		void flushStdout();
		void flushStderr();
		void flush();

		void checkErrors();

	public:
		void* userPtr = nullptr;
	};
}

#endif
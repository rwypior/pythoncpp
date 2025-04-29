#pragma once

#include "pythondefs.h"

#include "pythoncpp/PythonCpp.h"

#include <string>
#include <filesystem>
#include <unordered_map>

inline Python::Python& getPython()
{
	static bool initialized = false;

	std::string pydir = "python";

	if (!initialized)
	{
		Python::Initializer::getDefaultInitializer().setHome(Python::pythonhome);
	}

	Python::Python& py = Python::Python::getInstance();
	py.setPath(pydir);
	py.appendPath(Python::pythonhome);
	for(const auto& path : Python::pythondirs)
	{
		py.appendPath(std::string(path));
	}

	initialized = true;
	return py;
}

class TestSignal
{
public:
	static void emitSignal(const std::string& signalName)
	{
		auto it = signals.find(signalName);
		if (it == signals.end())
			signals[signalName] = 1;
		else
			it->second++;
	}

	static bool signalEmitted(const std::string& signalName)
	{
		return signalEmittedTimes(signalName) > 0;
	}

	static unsigned int signalEmittedTimes(const std::string& signalName)
	{
		auto it = signals.find(signalName);
		if (it == signals.end())
			return 0;
		unsigned int count = it->second;
		signals.erase(signalName);
		return count;
	}

private:
	static std::unordered_map<std::string, unsigned int> signals;
};

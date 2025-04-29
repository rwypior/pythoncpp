#pragma once

#include <string>

inline std::string getPythonDirectory()
{
	return "python";
}

inline std::string getDistDirectory()
{
	return "dist";
}

struct IExample
{
	virtual std::string getExampleName() const = 0;
	virtual void run() = 0;
};

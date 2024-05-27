#pragma once

#include <string>

inline std::string getPythonDirectory()
{
	return "python";
}

struct IExample
{
	virtual std::string getExampleName() const = 0;
	virtual void run() = 0;
};
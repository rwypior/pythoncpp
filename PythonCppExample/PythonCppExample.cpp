#include "common.h"

#include "basic_example.h"
#include "usage_example.h"
#include "create_function_example.h"
#include "create_module_example.h"
#include "create_class_example.h"
#include "python_class_example.h"
#include "run_code_example.h"
#include "module_reloading_example.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

int main()
{
	std::vector<std::unique_ptr<IExample>> examples;

	examples.push_back(std::make_unique<BasicExample>());
	examples.push_back(std::make_unique<UsageExample>());
	examples.push_back(std::make_unique<CreateFunctionExample>());
	examples.push_back(std::make_unique<CreateModuleExample>());
	examples.push_back(std::make_unique<CreateClassExample>());
	examples.push_back(std::make_unique<PythonClassExample>());
	examples.push_back(std::make_unique<RunCodeExample>());
	examples.push_back(std::make_unique<ModuleReloadingExample>());

	int choice = 0;

	do
	{
		std::cout << "Choose an example to run:" << std::endl;

		int i = 1;
		for (const auto& example : examples)
		{
			std::cout << "  " << i++ << " - " << example->getExampleName() << std::endl;
		}
		std::cout << "  0 - Exit the program" << std::endl;

		std::cin >> choice;

		if (choice < 1 || choice > examples.size())
		{
			std::cout << "Select an example between 1 and " << (examples.size() + 1) << std::endl;
			continue;
		}

		const auto& example = examples[choice - 1];

		std::cout << std::endl;
		std::cout << "==============================================================" << std::endl;
		std::cout << "Running " << example->getExampleName() << std::endl;
		std::cout << "==============================================================" << std::endl;
		std::cout << std::endl;

		example->run();

		std::cout << std::endl;
		std::cout << "==============================================================" << std::endl;
		std::cout << std::endl;
	}
	while (choice != 0);
}
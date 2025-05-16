#pragma once
#include <string>

class lang {
	bool hadError = false;
	std::string readFile(const std::string& path);
	void Run(std::string& soucre);
public:
	lang() = default;
	void init(int argc, char* argv[]);
	void RunFile(const char* path);
	void RunPrompt();
	void Report(int line, const char* message);
};
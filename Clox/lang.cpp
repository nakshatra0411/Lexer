#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "Scanner.h"

std::string lang::readFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) throw std::runtime_error("File not found");

	std::string content(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);
	return std::move(content);
}

void lang::Run(std::string& source) {
	
	Scanner* scanner = new Scanner(source,this);
	std::vector<Token> tokens = scanner->ScanTokens();

	for (auto& tok : tokens) {
		std::cout << tok.toString() << std::endl;
	}
}



void lang::init(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Usage: lang[script]";
		exit(64);
	}
	else if (argc == 2) {
		RunFile(argv[1]);
	}
	else {
		RunPrompt();
	}
}

void lang::RunFile(const char* path) {
	std::string source;
	try {
		 source = readFile(path);
		//std::cout << "Source file loaded (" << source.size() << " bytes)\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		exit(1);
	}
	Run(source);
	if (hadError) exit(65);
}

void lang::RunPrompt() {
	for (;;) {
		std::cout << "> ";
		std::string line; 
		std::getline(std::cin, line);
		if (line.empty()) break;
		Run(line);
		hadError = false;
	}
}

void lang::Report(int line, const char* message) {
	std::string temp = "[line " + std::to_string(line) + "] error" + ":" + message;
	std::cout << temp << std::endl;
	hadError = true;
}




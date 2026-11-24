#pragma once

#include <string>
#include <map>

class LineGenerator {
public:
	LineGenerator() = default;
	void setAxiom(std::string newAxiom);
	void setRules(std::map<char, std::string> newRules);
	std::string getLine(int iterations);
	int getLightLevel(std::string line);
private:
	std::string axiom = "";
	std::map<char, std::string> rules{};
};

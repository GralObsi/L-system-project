#include "linegenerator.h"

void LineGenerator::setAxiom(std::string newAxiom) {
	axiom = newAxiom;
}

void LineGenerator::setRules(std::map<char, std::string> newRules) {
	rules = newRules;
}

std::string LineGenerator::getLine(int iterations) {
	std::string line = axiom;
	for(int n = 0; n < iterations; n++) {
		std::string result = "";
		for(int unsigned i = 0; i < line.size(); i++) {
			auto iter = rules.find(line[i]);
			if(iter == rules.end()) result += line[i];
			else result += (*iter).second;
		}
		line = result;
	}
	return line;
}

int LineGenerator::getLightLevel(std::string line) {
	int curLevel = 0;
	int maxLevel = 0;
	for(int unsigned i = 0; i < line.size(); i++) {
		if(line[i] == '[') {
			curLevel += 1;
			if(curLevel > maxLevel) maxLevel = curLevel;
		} else if(line[i] == ']') {
			curLevel -= 1;
		}
	}
	return maxLevel;
}

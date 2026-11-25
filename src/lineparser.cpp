#include "lineparser.h"
#include "raymath.h"

float getRandFloat(float from, float to);

LineParser::LineParser(Turtle* turtlePtr): turtle(turtlePtr) {}

void LineParser::parse(std::string line) {
	int curLayer = 1;
	bool isRand = false;
	for(int unsigned i = 0; i < line.size(); i++) {
		bool r = isRand;
		isRand = false;
		char command = line[i];
		switch(command) {
			case '+':
				if(r) turtle->rotate(getRandFloat(std::min(0.0f, angle), std::max(0.0f, angle)));
				else turtle->rotate(angle);
				break;
			case '-':
				if(r) turtle->rotate(-getRandFloat(std::min(0.0f, angle), std::max(0.0f, angle)));
				else turtle->rotate(-angle);
				break;
			case '[':
				turtle->saveState();
				curLayer += 1;
				if(curLayer > 0 && curLayer <= maxLightLevel) {
					turtle->setPenColor(
						Color{ColorLerp(startColor, endColor, (float)curLayer / maxLightLevel)}
					);
				}
				break;
			case ']':
				turtle->loadState();
				curLayer -= 1;
				if(curLayer > 0 && curLayer <= maxLightLevel) {
					turtle->setPenColor(
						Color{ColorLerp(startColor, endColor, (float)curLayer / maxLightLevel)}
					);
				}
				break;
			case '^':
				turtle->penUp();
				break;
			case '_':
				turtle->penDown();
				break;
			case '~':
				isRand = true;
			default:
				turtle->forward(segmentLength * pow(decay, curLayer));
				break;
		}
	}
}

void LineParser::setColors(std::pair<Color, Color> colorPair) {
	startColor = colorPair.first;
	endColor = colorPair.second;
}

void LineParser::setAngle(float newAngle) {
	angle = newAngle;
}

void LineParser::setDecay(float newDecay) {
	decay = newDecay;
}

void LineParser::setMaxLightLevel(int newLightLevel) {
	maxLightLevel = newLightLevel;
}

float getRandFloat(float from, float to) {
	float num{0};
	float whole = GetRandomValue(floorf(from), ceilf(to));
	float afterPoint = (float)GetRandomValue(0, 100) / 100.0f;
	num = whole + afterPoint;
	if(num < from) num = from;
	if(num > to) num = to;
	return num;
}

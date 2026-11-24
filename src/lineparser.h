#pragma once

#include "turtle.h"
#include <string>

class LineParser {
public:
	LineParser() = default;
	LineParser(Turtle* turtlePtr);

	void parse(std::string line);
	void setColors(std::pair<Color, Color> colorPair);
	void setAngle(float newAngle);
	void setDecay(float newDecay);
	void setMaxLightLevel(int newLightLevel);
private:
	Turtle* turtle{};
	Color startColor = Color{58, 110, 87, 255};
	Color endColor = Color{182, 252, 221, 200};
	int maxLightLevel = 10;
	float segmentLength = 6.0;
	float decay = 1.0;
	float angle = 90.0;
};

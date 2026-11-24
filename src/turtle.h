#pragma once

#include "turtleline.h"
#include <stack>
#include <vector>

class Turtle {
public:
	Turtle() = default;
	Turtle(float x, float y, float angle);
	void forward(float length);
	void rotate(float angle);
	void saveState();
	void loadState();
	void penUp();
	void penDown();

	void setPenColor(Color color);
	void clear();
	void draw();

	TransMatrix transform{};
	void setScale(float scale);
	Vector2 getSystemSize();
	Vector2 getSystemCenter();
private:
	Color penColor = DARKGREEN;
	Vector2 penPosition{0.0, 0.0};
	float penAngle{90};
	bool isPenDowned = true;
	std::stack<std::pair<Vector2, float>> stateLog;

	Vector2 edges[2]{Vector2{0.0, 0.0}, Vector2{0.0, 0.0}};

	std::vector<TurtleLine> lines;
	void addLine(TurtleLine line);
};

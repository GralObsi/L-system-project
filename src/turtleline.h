#pragma once

#include "raylib.h"

struct TransMatrix {
public:
	TransMatrix() = default;
	TransMatrix(Vector2 pos, Vector2 scaleX, Vector2 scaleY);
	Vector2 orgin{0.0, 0.0};
	Vector2 x{1.0, 0.0};
	Vector2 y{0.0, 1.0};
};

struct TurtleLine {
public:
	TurtleLine() = default;
	TurtleLine(Vector2 iniStartPos, Vector2 iniEndPos, float iniThick, Color iniColor);
	void setLayer(int newLayer);
	void draw(TransMatrix transform);
	Color getColor();
private:
	Vector2 startPos;
	Vector2 endPos;
	float thick;
	Color color;
	int layer = 0;
};

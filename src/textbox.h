#pragma once

#include "raylib.h"
#include <string>

class TextBox {
public:
	TextBox() = default;
	TextBox(Vector2 newPos, Vector2 newSize, const char* newHeader, int newTextSize);
	void draw();
	std::string getString();
	void setString(std::string str);

	void setPosition(Vector2 newPosition);
	Vector2 getPosition();
	void setSize(Vector2 newSize);
	Vector2 getSize();
private:
	static const int COUNT = 256;
	bool active = false;
	char text[COUNT] = "";
	Vector2 position;
	Vector2 size;
	const char* header;
	int textSize = 1;
};

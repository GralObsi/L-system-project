#include "textbox.h"
#include "raygui.h"

TextBox::TextBox(Vector2 newPos, Vector2 newSize, const char* newHeader, int newTextSize):
	position(newPos), size(newSize), header(newHeader), textSize(newTextSize+1)
{}

void TextBox::draw() {
	if(GuiTextBox(
		(Rectangle) {position.x, position.y, size.x, size.y}, text, 16, active
	)) active = !active;
	DrawText(
		header, position.x + size.x + 4, position.y + (int)((size.y - 15) / 2.0f), 15, GREEN
	);
}

std::string TextBox::getString() {
	return std::string{text};
}

void TextBox::setString(std::string str) {
	for(int i = 0; i < COUNT; i++) {
		if(text[i] == '\0') break;
		text[i] = '\0';
	}
	for(unsigned int i = 0; i < str.size(); i++) {
		if(i == COUNT) break;
		text[i] = str[i];
	}
}

void TextBox::setPosition(Vector2 newPosition) {
	position = newPosition;
}

Vector2 TextBox::getPosition() {
	return position;
}

void TextBox::setSize(Vector2 newSize) {
	size = newSize;
}

Vector2 TextBox::getSize() {
	return size;
}

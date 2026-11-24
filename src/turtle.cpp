#include "turtle.h"
#include "raymath.h"

#include <string>

Turtle::Turtle(float x, float y, float angle): penAngle(angle) {
	transform.orgin.x = x;
	transform.orgin.y = y;
}

void Turtle::forward(float length) {
	Vector2 prev_position{Vector2{penPosition.x, penPosition.y}};
	penPosition.x -= length * cos(penAngle * DEG2RAD);
	penPosition.y -= length * sin(penAngle * DEG2RAD);
	
	edges[0].x = std::min(edges[0].x, penPosition.x);
	edges[0].y = std::min(edges[0].y, penPosition.y);
	edges[1].x = std::max(edges[1].x, penPosition.x);
	edges[1].y = std::max(edges[1].y, penPosition.y);

	TurtleLine line;
	if(isPenDowned) line = TurtleLine{
		prev_position, Vector2{penPosition.x, penPosition.y}, 2.0f, penColor
	};
	else line = TurtleLine{
		prev_position, Vector2{penPosition.x, penPosition.y}, 2.0f, Color{255, 255, 255, 0}
	};
	line.setLayer(stateLog.size());
	addLine(line);
}

void Turtle::rotate(float angle) {
	penAngle += angle;
}

void Turtle::saveState() {
	stateLog.push({penPosition, penAngle});
}

void Turtle::loadState() {
	penPosition = stateLog.top().first;
	penAngle = stateLog.top().second;
	stateLog.pop();
}

void Turtle::penUp() {
	isPenDowned = false;
}

void Turtle::penDown() {
	isPenDowned = true;
}

void Turtle::setPenColor(Color color) {
	penColor = color;
}

void Turtle::clear() {
	while(lines.size()) lines.pop_back();
	edges[0] = Vector2{0.0, 0.0}; edges[1] = Vector2{0.0, 0.0};
	penPosition = Vector2{0.0, 0.0};
	penAngle = 90.0;
}

void Turtle::draw() {
	for(int unsigned i = 0; i < lines.size(); i++) {
		if(lines[i].getColor().a != 0)lines[i].draw(transform);
	}
}

void Turtle::setScale(float scale) {
	transform.x = Vector2{1.0, transform.x.y / transform.x.x} * scale;
	transform.y = Vector2{transform.y.x / transform.y.y, 1.0} * scale;
}

Vector2 Turtle::getSystemSize() {
	Vector2 normalSize{edges[1] - edges[0]};
	return Vector2{normalSize.x * transform.x.x, normalSize.y * transform.y.y};
}

Vector2 Turtle::getSystemCenter() {
	Vector2 scaledEdge{edges[0].x * transform.x.x, edges[0].y * transform.y.y};
	return scaledEdge + getSystemSize() / 2.0;
}

void Turtle::addLine(TurtleLine line) {
	lines.push_back(line);
}

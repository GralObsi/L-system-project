#include "turtleline.h"

TransMatrix::TransMatrix(Vector2 pos, Vector2 scaleX, Vector2 scaleY):
	orgin(pos), x(scaleX), y(scaleY)
{}

TurtleLine::TurtleLine(Vector2 iniStartPos, Vector2 iniEndPos, float iniThick, Color iniColor):
	startPos(iniStartPos),
	endPos(iniEndPos),
	thick(iniThick),
	color(iniColor)
{}

void TurtleLine::setLayer(int newLayer) {
	layer = newLayer;
}

void TurtleLine::draw(TransMatrix transform) {
	Vector2 point_start{
		transform.orgin.x + (startPos.x * transform.x.x + startPos.y * transform.x.y),
		transform.orgin.y + (startPos.x * transform.y.x + startPos.y * transform.y.y)
	};
	Vector2 point_end{
		transform.orgin.x + (endPos.x * transform.x.x + endPos.y * transform.x.y),
		transform.orgin.y + (endPos.x * transform.y.x + endPos.y * transform.y.y)
	};
	DrawLineEx(point_start, point_end, thick, color);
}

Color TurtleLine::getColor() {
	return color;
}

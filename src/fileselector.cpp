#include "fileselector.h"
#include "raygui.h"

FileSelector::FileSelector(std::string directory) {
	setDirectory(directory);
}

void FileSelector::Draw() {
	if(!enable) return;
	/* -- DRAW -- */
	if(GuiWindowBox(
		(Rectangle){position.x, position.y, size.x, size.y}, "File Selector"
	)) toggleEnable();
	if(GuiListViewEx(
		(Rectangle){position.x, position.y + 24, size.x, size.y - 24},
		dirFiles.paths, dirFiles.count, &scrollIndex, &active, &focused
	)) {
		toggleEnable();
		selected = active;
		active = -1;
	}
}

void FileSelector::Update() {
	dirFiles = LoadDirectoryFiles(dirPath.c_str());
}

void FileSelector::toggleEnable() {
	enable = !enable;
}

bool FileSelector::isEnable() {
	return enable;
}

void FileSelector::setDirectory(std::string newDir) {
	dirPath = newDir;
	Update();
}

std::string FileSelector::getDirectory() {
	return dirPath;
}

void FileSelector::setPosition(Vector2 newPos) {
	position = newPos;
}

Vector2 FileSelector::getPosition() {
	return position;
}

void FileSelector::setSize(Vector2 newSize) {
	size = newSize;
}

Vector2 FileSelector::getSize() {
	return size;
}

void FileSelector::setSelected(int newSelected) {
	selected = newSelected;
}

int FileSelector::getSelected() {
	return selected;
}

char* FileSelector::getFile(int fileIdx) {
	return dirFiles.paths[fileIdx];
}

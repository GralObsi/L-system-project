#pragma once

#include "raylib.h"
#include <string>

class FileSelector {
public:
	FileSelector(std::string directory);
	void Draw();
	void Update();
	void toggleEnable();
	bool isEnable();

	void setDirectory(std::string newDir);
	std::string getDirectory();
	void setPosition(Vector2 newPos);
	Vector2 getPosition();
	void setSize(Vector2 newSize);
	Vector2 getSize();

	void setSelected(int newSelected);
	int getSelected();
	char* getFile(int fileIdx);
private:
	bool enable = false;

	std::string dirPath;
	FilePathList dirFiles;
	Vector2 position{0.0, 0.0};
	Vector2 size{200.0, 100.0};

	int scrollIndex{0};
	int active{-1};
	int focused{-1};

	int selected = -1;
};

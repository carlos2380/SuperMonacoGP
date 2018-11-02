#ifndef __MODULESCENESEGA_H__
#define __MODULESCENESEGA_H__


#include "Module.h"
#include "Animation.h"
#include "Point.h"

struct SDL_Texture;
class ModuleSceneSega : public Module
{

public:
	ModuleSceneSega(bool active = true);
	~ModuleSceneSega();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void checkNextsPoints();

public:
	SDL_Texture* selectSprites = nullptr;
	SDL_Texture* blackSprite = nullptr;

private:
	struct Line
	{
		bool toPrint;
		bool visited;
	};

	std::vector<std::vector<bool*>> matrixBorder;
	std::vector<Point<int>> pointsToPrintBorder, pointsToPrintContent;
	std::list<Point<int>> pointsToCheck;
	int timer = 0;
	SDL_Color contentColor;
	SDL_Rect background;
	SDL_Rect backgroundBlack;

};

#endif // __MODULESCENESEGA_H__
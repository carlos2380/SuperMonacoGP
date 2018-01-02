#ifndef __MODULESCENESEGA_H__
#define __MODULESCENESEGA_H__


#include "Module.h"
#include "Animation.h"
#include "Point.h"
using namespace std;

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

private:
	struct Line
	{
		bool toPrint;
		bool visited;
	};

	vector<vector<bool*>> matrixBorder;
	vector<Point<int>> pointsToPrintBorder, pointsToPrintContent;
	list<Point<int>> pointsToCheck;
	int timer = 0;
	SDL_Color contentColor;
	SDL_Rect background;

};

#endif // __MODULESCENESEGA_H__
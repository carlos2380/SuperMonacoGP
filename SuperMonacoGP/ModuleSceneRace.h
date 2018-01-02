#ifndef __MODULESCENERACE_H__
#define __MODULESCENERACE_H__


#include "Module.h"
#include "Animation.h"

struct SDL_Texture;
class ModuleSceneRace : public Module
{

public:
	ModuleSceneRace(bool active = true);
	~ModuleSceneRace();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void drawQuad(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
public:
	SDL_Texture* selectSprites = nullptr;

private:



};

#endif // __MODULESCENERACE_H__
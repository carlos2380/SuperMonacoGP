#ifndef __CTRLMAP_H__
#define __CTRLMAP_H__


#include "Module.h"
#include "Animation.h"

struct Line;
class CtrlMap : public Module
{

public:
	CtrlMap(bool active = true);
	CtrlMap(SDL_Texture* raceSprites, bool active = true);
	~CtrlMap();

	bool Start(ModuleSceneRace* &moduleRace);
	update_status Update();
	void drawPoligon(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
	void drawPoligonMirror(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
	bool CleanUp();

private:
	void loadRoad();

public:
	SDL_Texture* raceSprites = nullptr;
	SDL_Texture* skySprites = nullptr;
	ModuleSceneRace* moduleRace;

	int sizeMap;
	int segL = 600; //segment length
	float playerX = 0;
	float playerXMirror = 0;
	int worldPosition;
	int worldPositionMirror;
	int highWordl = 1500;
	int highWordlMirror = 1500;
	float speed = 0;
	float speedMirror = 0;
	int skyBoxX = 0;
	float sckyBoxY = 110.0;
	int sckyBoxXMirror = 2850;
	int mapPosition;
	int mapPositionMirror;
	int lastMapPosition;
	float turnAcceleration;


	//MIRROR

private:
	SDL_Rect skybox;
	SDL_Rect sky;
	SDL_Rect skyboxMirror;
};

#endif // __CTRLMAP_H__
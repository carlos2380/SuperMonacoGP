#ifndef __CTRLMAP_H__
#define __CTRLMAP_H__


#include "Module.h"
#include "Animation.h"
#include <map>

struct Line;
class CtrlMap : public Module
{

public:
	CtrlMap(bool active = true);
	~CtrlMap();

	bool Start();
	update_status Update();
	void drawPoligon(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
	void drawPoligonMirror(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b);
	bool CleanUp();

private:
	void loadRoad();

public:
	SDL_Texture* raceSprites = nullptr;
	SDL_Texture* skySprites = nullptr;

	int sizeMap;
	int segL = 600; //segment length
	float playerX = 0;
	float playerXMirror = 0;
	int worldPosition;
	int worldPositionMirror;
	int highWordl = 1500;
	int highWordlMirror = 1700;
	float speed = 0;
	float speedMirror = 0;
	int skyBoxX = 0;
	float sckyBoxY = 110.0;
	int sckyBoxXMirror = 2850;
	int mapPosition;
	int mapPositionMirror;
	int lastMapPosition;
	float turnAcceleration;
	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int height = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
	int heightMirror = 40 * SCREEN_SIZE;
	int roadW = 4000;
	float camD = 0.84f; //camera depth


	//MIRROR

private:
	SDL_Rect skybox;
	SDL_Rect sky;
	SDL_Rect skyboxMirror;
	std::vector<SDL_Rect*> spriteVector;
	std::vector<Line> mapLines;
	std::vector<Line> mapMirror;
	//SDL_Rect whells;
};

#endif // __CTRLMAP_H__
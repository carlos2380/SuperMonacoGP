#ifndef __LINE_H__
#define __LINE_H__
#include <SDL.h>
#include <list> 
#include "Color.h"

struct AICar;
class Line
{
public:
	Line();
	~Line();
	void project(int camX, int camY, int camZ);
	void projectMirror(int camX, int camY, int camZ);
	void drawSprite(SDL_Texture* tex);
	void drawSpriteMirror(SDL_Texture* tex);
	void drawCars();
	void drawCarsMirror();
	void addCar(AICar* car);
	void removeCar(AICar* car);

public:
	float worldX, worldY, worldZ; //3d center of line
	float screenX, screenY, screenW; //screen coord
	float curve, spriteX, clip, scale;
	bool isTunnel;
	Color grass, border, road;
	SDL_Rect* sprite = nullptr;
	std::list<AICar*> cars;

	//Gived by CtrlMap
	int width;
	int height;
	int heightMirror;
	int roadW;
	float camD; 
};

#endif // __LINE_H__
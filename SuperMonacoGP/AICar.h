#ifndef __AICAR_H__
#define __AICAR_H__
#include <SDL.h>

struct Line;
class AICar
{
public:
	AICar();
	~AICar();
private:

public:
	Line* line;
	int linePos;
	SDL_Rect* sprite = nullptr;
	SDL_Rect* spriteMirror = nullptr;
	SDL_Texture* tex = nullptr;;
	float spriteX;
	int lap = 1;//0;

private:
	int speed;
	float turn;

};

#endif // __AICAR_H__
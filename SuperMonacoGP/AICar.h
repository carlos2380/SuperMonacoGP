#ifndef __AICAR_H__
#define __AICAR_H__
#include <SDL.h>

struct Line;
class AICar
{
public:
	AICar();
	~AICar();
	Line* line;
	int linePos;
	SDL_Rect* sprite = nullptr;
	SDL_Rect* spriteMirror = nullptr;

private:
	int speed;
	float spriteX;
	float turn;

};

#endif // __AICAR_H__
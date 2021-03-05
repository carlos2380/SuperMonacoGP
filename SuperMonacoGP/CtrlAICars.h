#ifndef __CTRLAICARS_H__
#define __CTRLAICARS_H__
#include <SDL.h>
#include <vector>
#include "Module.h"

struct AICar;
struct Line;
class CtrlAICars : public Module
{
public:
	CtrlAICars(bool active = true);
	~CtrlAICars();

	bool Start();
	update_status Update();
	bool CleanUp();
private:

public:
	std::vector<AICar*> AICars;
	std::vector<Line>* mapLines;
	std::vector<Line>* mapMirror;
	SDL_Texture* texSprite = nullptr;
	int sizeMap;

private:
	std::vector<SDL_Rect*> CarAIVector;
	std::vector<SDL_Rect*> CarAIMirrorVector;
};

#endif // __CTRLAICARS_H__
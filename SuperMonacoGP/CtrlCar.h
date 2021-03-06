#ifndef __CTRLCAR_H__
#define __CTRLCAR_H__


#include "Module.h"
#include "Animation.h"

struct SDL_Texture;
enum mode
{
	AUTOMATIC = 3,
	MANUAL = 4,
	SUPER = 7
};
class CtrlCar : public Module
{

public:
	CtrlCar(bool active = true);
	CtrlCar(SDL_Texture* raceSprites, bool active = true);
	~CtrlCar();

	bool Start();
	update_status Update();
	bool CleanUp();
	void setGear(mode gear);

private:
	void loadRects();
	void unloadRects();
	void findRectToPrint(SDL_Rect* &rectFrame, int frame);
	void updateTurn();
	void updateSpeed();
	void updateGear();
	void updateRevol();

public:
	SDL_Texture* raceSprites = nullptr;
	float speed;
	float revol;	
	mode gear;
	int gearSelect;
	
private:
	int tick;
	int maxSpeed;
	int frame;
	int speedFrame;
	std::vector<SDL_Rect*> front;
	std::vector<SDL_Rect*> left;
	std::vector<SDL_Rect*> leftLeft;
	std::vector<SDL_Rect*> leftLeftLeft;
	std::vector<SDL_Rect*> right;
	std::vector<SDL_Rect*> rightRight;
	std::vector<SDL_Rect*> rightRrightRight;
	int turn;
};

#endif // __CTRLCAR_H__
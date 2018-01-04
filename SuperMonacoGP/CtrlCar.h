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
public:
	SDL_Texture* raceSprites = nullptr;
	float speed;
	float revol;	
	
	
private:
	int tick;
	mode gear;
	int maxSpeed;
	int gearSelect;
	int frame;
	int speedFrame;
	vector<SDL_Rect*> front;
	vector<SDL_Rect*> left;
	vector<SDL_Rect*> leftLeft;
	vector<SDL_Rect*> leftLeftLeft;
	vector<SDL_Rect*> right;
	vector<SDL_Rect*> rightRight;
	vector<SDL_Rect*> rightRrightRight;
	int turn;
};

#endif // __CTRLCAR_H__
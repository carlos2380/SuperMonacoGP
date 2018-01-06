#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include "CtrlCar.h"
#include <iostream>


CtrlCar::CtrlCar(bool active) : Module(active)
{
}

CtrlCar::CtrlCar(SDL_Texture* rs, bool active) : Module(active)
{
}


CtrlCar::~CtrlCar()
{}

// Load assets
bool CtrlCar::Start()
{
	
	raceSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);
	LOG("Loading space intro");
	loadRects();
	speed = 0;
	turn = 0;
	gearSelect = 1;
	gear = SUPER;
	frame = 0;
	tick = 1;
	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	setGear(SUPER);
	return true;
}


// UnLoad assets
bool CtrlCar::CleanUp()
{
	LOG("Unloading space scene");
	raceSprites = nullptr;
	unloadRects();
	return true;
}

update_status CtrlCar::Update()
{
	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	//cout << mx << ": " << my << endl;*/

	updateTurn();
	updateSpeed();
	updateGear();

	//Normalize speed and get revolutions
	if (speed > maxSpeed) speed = maxSpeed;
	if (speed < 0) speed = 0;

	updateRevol();

	//DEBUG: if(tick%20==0) cout << speed << " : " << revol << " : " << gearSelect << endl;

	
	++tick;
	if (speed > 0 && tick%(speedFrame -((int)speed/50))== 0)
	{
		frame = (frame + 1) % 3;
	}

	SDL_Rect* rectFrame ;
	findRectToPrint(rectFrame, frame);
	
	App->renderer->Blit(raceSprites, 64, 209, rectFrame, NULL, 1.3);

	return UPDATE_CONTINUE;
}

void CtrlCar::updateTurn()
{
	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
	{
		turn -= 60;
	}
	else if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
	{
		turn += 60;
	}
	else
	{
		if (turn > 0) turn -= 20;
		if (turn < 0) turn += 20;
	}
}

void CtrlCar::updateSpeed()
{
	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_U) == KEY_REPEAT)
	{
		switch (gear)
		{
		case AUTOMATIC:
			speed += 0.5;
			break;
		case MANUAL:
			speed += 0.8 - (gearSelect*0.1);
			break;
		case SUPER:
			speed += 1.2 - (gearSelect*0.1);
			break;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		switch (gear)
		{
		case AUTOMATIC:
			speed -= 1;
			break;
		case MANUAL:
			speed -= 1 - (gearSelect*0.1);
			break;
		case SUPER:
			speed -= 1 - (gearSelect*0.1);
			break;
		}
	}
	else
	{
		speed -= 0.3;
	}
}

void CtrlCar::updateGear()
{
	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		switch (gear)
		{
		case AUTOMATIC:
			break;
		case MANUAL:
		case SUPER:
			if (gearSelect > 1) --gearSelect;
			break;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		switch (gear)
		{
		case AUTOMATIC:
			break;
		case MANUAL:
			if (gearSelect < MANUAL) ++gearSelect;
			break;
		case SUPER:
			if (gearSelect < SUPER) ++gearSelect;
			break;
		}
	}


	if (gear == AUTOMATIC)
	{
		if (speed < (gearSelect*(maxSpeed / AUTOMATIC)))
		{
			if (gearSelect > 1) --gearSelect;
		}
		if (speed > (gearSelect*(maxSpeed / AUTOMATIC)))
		{
			if (gearSelect < MANUAL) ++gearSelect;
		}
	}
	else
	{
		if (speed >(gearSelect*(maxSpeed / gear) - 1)) speed = (gearSelect*(maxSpeed / gear) - 1);
	}
}

void CtrlCar::updateRevol()
{
	//Revol from 0 to 90, are de degree. Then less operations for cpu.
	if (((maxSpeed / gear)*(gearSelect - 1)) > speed)
	{
		revol = 0;
	}
	else if (((maxSpeed / gear)*gearSelect) < speed)
	{
		revol = 90;
	}
	else {
		revol = (((int)speed % (maxSpeed / gear)) * 90) / (maxSpeed / gear);
	}

}


void CtrlCar::findRectToPrint(SDL_Rect* &rectFrame, int frame)
{
	if (turn == 0)
	{
		rectFrame = front[frame];
	}
	else if (turn > 1440)
	{
		rectFrame = leftLeftLeft[frame];
		if (turn > 1500) turn = 1500;
	}
	else if (turn > 300)
	{
		rectFrame = leftLeft[frame];
	}
	else if (turn > 0)
	{
		rectFrame = left[frame];
	}
	else if (turn < -1440)
	{
		rectFrame = rightRrightRight[frame];
		if (turn < -1500) turn = -1500;
	}
	else if (turn < -300)
	{
		rectFrame = rightRight[frame];
	}
	else if (turn < 0)
	{
		rectFrame = right[frame];
	}
	else
	{
		rectFrame = front[frame];
	}
}


void CtrlCar::setGear(mode gear)
{
	this->gear = gear;
	switch (gear)
	{
	case AUTOMATIC:
		maxSpeed = 310;
		speedFrame = 7;
	break;
	case MANUAL:
		maxSpeed = 330;
		speedFrame = 7;
		break;
	case SUPER:
		maxSpeed = 370;
		speedFrame = 8;
		break;
	}
}


void CtrlCar::loadRects()
{
	front = vector<SDL_Rect*>(3);
	left = vector<SDL_Rect*>(3);
	leftLeft = vector<SDL_Rect*>(3);
	leftLeftLeft = vector<SDL_Rect*>(3);
	right = vector<SDL_Rect*>(3);
	rightRight = vector<SDL_Rect*>(3);
	rightRrightRight = vector<SDL_Rect*>(3);

	for (int i = 0; i < 3; ++i)
	{
		front[i] = new SDL_Rect();
		left[i] = new SDL_Rect();
		leftLeft[i] = new SDL_Rect();
		leftLeftLeft[i] = new SDL_Rect();
		right[i] = new SDL_Rect();
		rightRight[i] = new SDL_Rect();
		rightRrightRight[i] = new SDL_Rect();
		front[i]->x = left[i]->x = leftLeft[i]->x = leftLeftLeft[i]->x = right[i]->x = rightRight[i]->x = rightRrightRight[i]->x = 0;
		front[i]->h = left[i]->h = leftLeft[i]->h = leftLeftLeft[i]->h = right[i]->h = rightRight[i]->h = rightRrightRight[i]->h = 24;
		front[i]->w = left[i]->w = leftLeft[i]->w = leftLeftLeft[i]->w = right[i]->w = rightRight[i]->w = rightRrightRight[i]->w = 147;
		front[i]->y = 325 + i * 24;
		left[i]->y = front[i]->y + 72;
		leftLeft[i]->y = left[i]->y + 72;
		leftLeftLeft[i]->y = leftLeft[i]->y + 72;
		right[i]->y = leftLeftLeft[i]->y + 72;
		rightRight[i]->y = right[i]->y + 72;
		rightRrightRight[i]->y = rightRight[i]->y + 72;
	}
}


void CtrlCar::unloadRects()
{
	for (int i = 0; i < front.size(); ++i)
	{
		front[i] = nullptr;
		left[i] = nullptr;
		leftLeft[i] = nullptr;
		leftLeftLeft[i] = nullptr;
		right[i] = nullptr;
		rightRight[i] = nullptr;
		rightRrightRight[i] = nullptr;
		delete front[i];
		delete left[i];
		delete leftLeft[i];
		delete leftLeftLeft[i];
		delete right[i];
		delete rightRight[i];
		delete rightRrightRight[i];
	}
}

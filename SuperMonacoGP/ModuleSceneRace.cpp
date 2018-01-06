#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include "SDL2_gfxPrimitives.h"
#include <ostream>
#include <iostream>

int width = SCREEN_WIDTH*SCREEN_SIZE;
int height = SCREEN_HEIGHT*SCREEN_SIZE;
int roadW = 2000;
int segL = 200; //segment length
float camD = 0.84; //camera depth
int N ;
float playerX = 0;
int pos = 0;
int H = 1500;
int speed = 0;

struct Line
{
	float x, y, z; //3d center of line
	float X, Y, W; //screen coord
	float curve, spriteX, clip, scale;

	Line()
	{
		spriteX = curve = x = y = z = 0;
	}

	void project(int camX, int camY, int camZ)
	{
		scale = camD / (z - camZ);
		X = (1 + scale*(x - camX)) * width / 2;
		Y = (1 - scale*(y - camY)) * height / 2;
		W = scale * roadW  * width / 2;
	}

};

vector<Line> lines;

//Get mouse position

ModuleSceneRace::ModuleSceneRace(bool active) : Module(active)
{
	background = { 0, 72, 320, 240 };
}

ModuleSceneRace::~ModuleSceneRace()
{}

// Load assets
bool ModuleSceneRace::Start()
{
	LOG("Loading space intro");
	
	

	for (int i = 0; i<1600; i++)
	{
		Line line;
		line.z = i*segL;

		if (i>300 && i<700) line.curve = 0.5;
		if (i>1100) line.curve = -0.7;


		if (i>750) line.y = sin(i / 30.0) * 1500;

		lines.push_back(line);
	}

	N = lines.size();
	playerX = 0;
	pos = 0;
	H = 1500;
	raceSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);

	ctrlCar = new CtrlCar(raceSprites);
	ctrlCar.Start();

	ctrlUI = new CtrlUI();
	ctrlUI.ctrlCar = &ctrlCar;
	ModuleSceneRace* moduleRace = this;
	ctrlUI.Start(moduleRace);
	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

// UnLoad assets
bool ModuleSceneRace::CleanUp()
{
	LOG("Unloading space scene");
	ctrlCar.CleanUp();
	ctrlUI.CleanUp();
	App->textures->Unload(raceSprites);
	return true;
}

// Update: draw background
update_status ModuleSceneRace::Update()
{

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		speed = 200;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		speed = -200;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		playerX += 0.1;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		playerX -= 0.1;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		H += 100;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		H -= 100;
	}
	
	pos += speed;
	while (pos >= N*segL) pos -= N*segL;
	while (pos < 0) pos += N*segL;

	int startPos = pos / segL;
	int camH = lines[startPos].y + H;

	int maxy = height;
	float x = 0, dx = 0;
	
	for (int n = startPos; n<startPos + 100; n++)
	{
		Line &l = lines[n%N];
		l.project(playerX*roadW - x, camH, startPos*segL - (n >= N ? N*segL : 0));
		x += dx;
		dx += l.curve;

		l.clip = maxy;
		if (l.Y >= maxy) continue;
		maxy = l.Y;
		int rg, ru, ro, gg, gu, go, bg, bu, bo;

		if ((n / 3) % 2) {
			rg = 16;
			gg = 200;
			bg = 16;
		}
		else {
			rg = 0;
			gg = 154;
			bg = 0;
		}

		if ((n / 3) % 2) {
			ru = 255;
			gu = 255;
			bu = 255;
		}
		else {
			ru = 0;
			gu = 0;
			bu = 0;
		}

		if ((n / 3) % 2) {
			ro = 107;
			go = 107;
			bo = 107;
		}
		else {
			ro= 105;
			go= 105;
			bo= 105;
		}

		

		Line p = lines[(n - 1) % N]; //previous line

		drawQuad( 0, p.Y, width, 0, l.Y, width, rg, gg, bg);
		drawQuad( p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2, ru, gu, bu);
		drawQuad( p.X, p.Y, p.W, l.X, l.Y, l.W, ro, go, bo);
	}
	App->renderer->Blit(raceSprites, 0, 0, &background);

	ctrlCar.Update();
	int lap = 2;
	ctrlUI.Update();

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_start, this);
		//App->audio->PlayFx(fx);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneRace::drawQuad(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	short x[4] = { x1-w1, x2-w2, x2+w2, x1+w1 };
	short y[4] = { y1, y2, y2, y1 };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}
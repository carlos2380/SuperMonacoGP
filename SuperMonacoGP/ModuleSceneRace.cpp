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
int height = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
int roadW = 4000;
int segL = 600; //segment length
float camD = 0.84; //camera depth
int N;
float playerX = 0;
int pos = 1000;
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
	skybox = { 0, 0, 320, 200 };
}

ModuleSceneRace::~ModuleSceneRace()
{}

// Load assets
bool ModuleSceneRace::Start()
{
	LOG("Loading space intro");

	float descens = 0;
	for (int i = 0; i<2700; i++)
	{
		Line line;
		line.z = i*segL;

		if (i>300 && i<420) line.curve = 15;
		if (i>420 && i<450) line.curve = 5;
		if (i>450 && i<550) line.curve = 20;
		if (i>600 && i<750) line.curve = 3;
		if (i>750 && i<820) line.curve = -10;
		if (i>820 && i<950) line.curve = 10;
		if (i>950 && i<1000) line.curve = -15;
		if (i>1100 && i<1200) line.curve = -5;
		if (i>1200 && i<1230) line.curve = -20;

		if (i>1300 && i<1320) line.curve = -50;
		if (i>1320 && i<1340) line.curve = 50;

		if (i>1400 && i<1420) line.curve = 50;
		if (i>1420 && i<1440) line.curve = -50;

		if (i>1550 && i<1650) line.curve = -5;

		if (i>1650 && i<1850) line.curve = 20;
		if (i>1850 && i<2050) line.curve = -10;

		if (i>2150 && i<2200) line.curve = 70;

		if (i>2300 && i<2350) line.curve = -15;
		if (i>2350 && i<2400) line.curve = 15;
		


		
		if (i>1050 && i<1200) descens += -150;
		if (i>1450 && i<1550) descens += 200;
		if (i>1700 && i<1750) descens += 150;
		if (i>1750 && i<1800) descens += 300;
		if (i>1810 && i<2006) descens -= 100;
		line.y = descens;

		//if (i>0) line.y = sin(i / 40.0) * 4500;
		lines.push_back(line);
	}

	N = lines.size();
	playerX = 0;
	pos = 0;
	H = 1500;
	skySprites = App->textures->Load("Sprites/SkyBox.bmp", 255, 0, 255);
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

int skyBoxX = 0;
int nextPixel = 0;
// Update: draw background
float posS = 110.0;
update_status ModuleSceneRace::Update()
{

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		speed = 400;
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_V) == KEY_REPEAT)
	{
		speed = 0;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		speed = -400;
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

	startPos = pos / segL;
	int camH = lines[startPos].y + H;

	int maxy = height;
	float x = 0, dx = 0;
	App->renderer->Blit(skySprites, 0, posS, &skybox);
	if (lines[startPos%N].curve != 0) {
		playerX += -lines[startPos%N].curve*0.003;
	}

	if (lines[startPos%N].y < lines[(startPos+1)%N].y) {
		posS -= 0.1;
	}
	else if (lines[startPos%N].y > lines[(startPos + 1) % N].y) {
		posS += 0.1;
	}else if (posS > 112)
	{
		posS -=0.5;
	}
	else if (posS < 108)
	{
		posS += 0.5;
	}else
	{
		posS = 110;
	}
	
	for (int n = startPos; n<startPos + 65; n++)
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
			rg = 0;
			gg = 180;
			bg = 0;
		}
		else {
			rg = 0;
			gg = 144;
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
			bu = 248;
		}

		if ((n / 3) % 2) {
			ro = 107;
			go = 107;
			bo = 107;
		}
		else {
			ro = 105;
			go = 105;
			bo = 105;
		}



		Line p = lines[(n - 1) % N]; //previous line

		drawQuad(0, p.Y, width, 0, l.Y, width, rg, gg, bg);
		drawQuad(p.X, p.Y, p.W*1.3, l.X, l.Y, l.W*1.3, ru, gu, bu);
		drawQuad(p.X, p.Y, p.W, l.X, l.Y, l.W, ro, go, bo);
		drawQuad(p.X, p.Y, p.W*0.9, l.X, l.Y, l.W*0.9, 255, 255, 255);
		drawQuad(p.X, p.Y, p.W*0.8, l.X, l.Y, l.W*0.8, ro, go, bo);
		if(n%6 < 2) drawQuad(p.X, p.Y, p.W*0.05, l.X, l.Y, l.W*0.05, 255, 255, 255);
	}
	
	if (speed > 0) {
		skyBoxX += lines[startPos].curve * 0.1f;
	}
	if (speed<0) skyBoxX -= lines[startPos].curve * 0.1f;
	skybox.x = skyBoxX;


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
	int desp = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
	short x[4] = { x1 - w1, x2 - w2, x2 + w2, x1 + w1 };
	short y[4] = { y1 + desp, y2 + desp, y2 + desp, y1 + desp };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}
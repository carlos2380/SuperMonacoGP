#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneRace.h"
#include <SDL.h>
#include "CtrlMap.h"
#include <iostream>
#include "SDL2_gfxPrimitives.h"


int width = SCREEN_WIDTH*SCREEN_SIZE;
int height = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
int heightMirror = 40 * SCREEN_SIZE;
int roadW = 4000;
float camD = 0.84; //camera depth

struct Color
{
	int r, g, b;
};

struct Line
{
	float worldX, worldY, worldZ; //3d center of line
	float screenX, screenY, screenW; //screen coord
	float curve, spriteX, clip, scale;
	Color grass, border, road;

	Line()
	{
		spriteX = curve = worldX = worldY = worldZ = 0;
	}

	void project(int camX, int camY, int camZ)
	{
		scale = camD / (worldZ - camZ);
		screenX = (1 + scale*(worldX - camX)) * width / 2;
		screenY = (1 - scale*(worldY - camY)) * height / 2;
		screenW = scale * roadW  * width / 2;
	}

	void projectMirror(int camX, int camY, int camZ)
	{
		scale = camD / (worldZ - camZ);
		screenX = (1 + scale*(worldX - camX)) * width / 2;
		screenY = (1 - scale*(worldY - camY)) * heightMirror / 2;
		screenW = scale * roadW  * width / 2;
	}

};
vector<Line> map;
vector<Line> mapMirror;

CtrlMap::CtrlMap(bool active) : Module(active)
{
}

CtrlMap::CtrlMap(SDL_Texture* rs, bool active) : Module(active)
{
}


CtrlMap::~CtrlMap()
{}

// Load assets
bool CtrlMap::Start(ModuleSceneRace* &moduleRace)
{
	this->moduleRace = moduleRace;
	turnAcceleration = 0;
	skybox = { 0, 0, 320, 200 };
	skyboxMirror = { 800, 0, 320, 100 };
	sky = { 0, 300, 320, 100 };

	loadRoad();

	sizeMap = map.size();
	playerX = 0;
	playerXMirror = 0;
	worldPosition = -50000;
	worldPositionMirror = +50000;
	lastMapPosition = mapPosition;
	skySprites = App->textures->Load("Sprites/SkyBox.bmp", 255, 0, 255);
	raceSprites = App->textures->Load("Sprites/RaceSprites.bmp", 255, 0, 255);
	LOG("Loading space intro");

	/*if (fx == 0)
	fx = App->audio->LoadFx("rtype/starting.wav");*/

	return true;
}


// UnLoad assets
bool CtrlMap::CleanUp()
{
	LOG("Unloading space scene");
	raceSprites = nullptr;

	return true;
}

update_status CtrlMap::Update()
{

	speed = moduleRace->ctrlCar.speed*1.2;
	speedMirror = -speed;
	//UPDATE TURN
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turnAcceleration < 0) turnAcceleration = 0;
		if (turnAcceleration < 0.0004) turnAcceleration += 0.000003;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turnAcceleration > 0) turnAcceleration = 0;
		if (turnAcceleration > -0.0004) turnAcceleration -= 0.000003;

	}
	else
	{
		if (turnAcceleration < 0) turnAcceleration += 0.0001;
		else if (turnAcceleration > 0) turnAcceleration -= 0.0001;
	}
	playerX += turnAcceleration*speed;
	playerXMirror += turnAcceleration*speed;

	worldPosition += speed;
	while (worldPosition >= sizeMap*segL) worldPosition -= sizeMap*segL;
	while (worldPosition < 0) worldPosition += sizeMap*segL;

	worldPositionMirror += speedMirror;
	while (worldPositionMirror >= sizeMap*segL) worldPositionMirror -= sizeMap*segL;
	while (worldPositionMirror < 0) worldPositionMirror += sizeMap*segL;

	mapPosition = worldPosition / segL;
	int camH = map[mapPosition].worldY + highWordl;

	mapPositionMirror = worldPositionMirror / segL;
	int camHMirror = mapMirror[mapPositionMirror].worldY + highWordlMirror;

	int maxy = height;
	int maxyMirror = height;
	float x = 0, dx = 0;
	
	if (map[mapPosition%sizeMap].curve != 0) {
		playerX += -map[mapPosition%sizeMap].curve*0.003;
		playerXMirror += -map[mapPosition%sizeMap].curve*0.003;
	}
	App->renderer->Blit(skySprites, 0, -20, &skyboxMirror);

	for (int n = mapPositionMirror; n<mapPositionMirror + 40; n++)
	{
		Line &l = mapMirror[n%sizeMap];
		l.projectMirror(playerXMirror*roadW - x, camHMirror, mapPositionMirror*segL - (n >= sizeMap ? sizeMap*segL : 0));
		x += dx;
		dx += l.curve;

		l.clip = maxyMirror;
		if (l.screenY >= maxyMirror) continue;
		maxyMirror = l.screenY;



		Line p = mapMirror[(n - 1) % sizeMap]; //previous line
		if (n  > mapPositionMirror +3) {
			drawPoligonMirror(0, p.screenY, width, 0, l.screenY, width, l.grass.r, l.grass.g, l.grass.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*1.3, l.screenX, l.screenY, l.screenW*1.3, l.border.r, l.border.g, l.border.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW, l.screenX, l.screenY, l.screenW, l.road.r, l.road.g, l.road.b);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.9, l.screenX, l.screenY, l.screenW*0.9, 255, 255, 255);
			drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, l.road.r, l.road.g, l.road.b);
			if (n % 6 < 2) drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
			if (n%sizeMap == sizeMap - 1 || n%sizeMap == sizeMap - 3) {
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 255, 255, 255);
			}
			if (n%sizeMap == sizeMap - 2) {
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 255, 255, 255);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 0, 0, 0);
				drawPoligonMirror(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
			}
		}
	}
	App->renderer->Blit(skySprites, 0, 65, &sky);
	App->renderer->Blit(skySprites, 0, sckyBoxY, &skybox);
	//UPDATE NEXT SKYBOX Y
	if (map[mapPosition%sizeMap].worldY < map[(mapPosition + 1) % sizeMap].worldY) {
		sckyBoxY -= 0.1;
	}
	else if (map[mapPosition%sizeMap].worldY > map[(mapPosition + 1) % sizeMap].worldY) {
		sckyBoxY += 0.1;
	}
	else if (sckyBoxY > 112)
	{
		sckyBoxY -= 0.5;
	}
	else if (sckyBoxY < 108)
	{
		sckyBoxY += 0.5;
	}
	else
	{
		sckyBoxY = 110;
	}
	x = 0;
	dx = 0;
	//UPDATE ROAD
	for (int n = mapPosition; n<mapPosition + 65; n++)
	{
		Line &l = map[n%sizeMap];
		l.project(playerX*roadW - x, camH, mapPosition*segL - (n >= sizeMap ? sizeMap*segL : 0));
		x += dx;
		dx += l.curve;

		l.clip = maxy;
		if (l.screenY >= maxy) continue;
		maxy = l.screenY;



		Line p = map[(n - 1) % sizeMap]; //previous line

		drawPoligon(0, p.screenY, width, 0, l.screenY, width, l.grass.r, l.grass.g, l.grass.b);
		drawPoligon(p.screenX, p.screenY, p.screenW*1.3, l.screenX, l.screenY, l.screenW*1.3, l.border.r, l.border.g, l.border.b);
		drawPoligon(p.screenX, p.screenY, p.screenW, l.screenX, l.screenY, l.screenW, l.road.r, l.road.g, l.road.b);
		drawPoligon(p.screenX, p.screenY, p.screenW*0.9, l.screenX, l.screenY, l.screenW*0.9, 255, 255, 255);
		drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, l.road.r, l.road.g, l.road.b);
		if (n % 6 < 2) drawPoligon(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
		if (n%sizeMap == 0 || n%sizeMap == 2) {
			drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 255, 255, 255);
		}
		if (n%sizeMap == 1) {
			drawPoligon(p.screenX, p.screenY, p.screenW*0.8, l.screenX, l.screenY, l.screenW*0.8, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.6, l.screenX, l.screenY, l.screenW*0.6, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.4, l.screenX, l.screenY, l.screenW*0.4, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.2, l.screenX, l.screenY, l.screenW*0.2, 255, 255, 255);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.1, l.screenX, l.screenY, l.screenW*0.1, 0, 0, 0);
			drawPoligon(p.screenX, p.screenY, p.screenW*0.05, l.screenX, l.screenY, l.screenW*0.05, 255, 255, 255);
		}
	}
	


	//UPDATE NEXT SKYBOX X
	if (speed > 0) {
		skyBoxX += map[mapPosition].curve * 0.1f;
		sckyBoxXMirror -= map[mapPosition].curve * 0.1f;
	}
	if (speed < 0) {
		skyBoxX -= map[mapPosition].curve * 0.1f;
		sckyBoxXMirror += map[mapPosition].curve * 0.1f;
	}
	skybox.x = skyBoxX;
	skyboxMirror.x = sckyBoxXMirror;
	


	if (mapPosition == 0 && lastMapPosition != mapPosition) moduleRace->nextLap();
	lastMapPosition = mapPosition;
	//App->renderer->Blit(raceSprites, 64, 209, rectFrame, NULL, 1.3);

	return UPDATE_CONTINUE;
}

void CtrlMap::drawPoligon(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	int desp = (SCREEN_HEIGHT / 2)*SCREEN_SIZE;
	short x[4] = { x1 - w1, x2 - w2, x2 + w2, x1 + w1 };
	short y[4] = { y1 + desp, y2 + desp, y2 + desp, y1 + desp };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}

void CtrlMap::drawPoligonMirror(int x1, int y1, int w1, int x2, int y2, int w2, int r, int g, int b)
{
	int desp = 31 * SCREEN_SIZE;
	short x[4] = { x1 - w1, x2 - w2, x2 + w2, x1 + w1 };
	short y[4] = { y1 + desp, y2 + desp, y2 + desp, y1 + desp };

	filledPolygonRGBA(App->renderer->renderer, x, y, 4, r, g, b, 255);
}

void CtrlMap::loadRoad()
{
	float descens = 0;
	for (int i = 0; i < 2700; i++)
	{
		Line line;
		line.worldZ = i*segL;

		if (i > 300 && i < 420) line.curve = 15;
		if (i > 420 && i < 450) line.curve = 5;
		if (i > 450 && i < 550) line.curve = 20;
		if (i > 600 && i < 750) line.curve = 3;
		if (i > 750 && i < 820) line.curve = -10;
		if (i > 820 && i < 950) line.curve = 10;
		if (i > 950 && i < 1000) line.curve = -15;
		if (i > 1100 && i < 1200) line.curve = -5;
		if (i > 1200 && i < 1230) line.curve = -20;

		if (i > 1300 && i < 1320) line.curve = -30;
		if (i > 1320 && i < 1340) line.curve = 30;

		if (i > 1400 && i < 1420) line.curve = 30;
		if (i > 1420 && i < 1440) line.curve = -30;

		if (i > 1550 && i < 1650) line.curve = -5;

		if (i > 1650 && i < 1850) line.curve = 20;
		if (i > 1850 && i < 2050) line.curve = -10;

		if (i > 2100 && i < 2250) line.curve = 25;

		if (i > 2300 && i < 2350) line.curve = -15;
		if (i > 2350 && i < 2400) line.curve = 15;




		if (i > 1050 && i < 1200) descens += 150;
		if (i > 1450 && i < 1550) descens += -200;
		if (i > 1700 && i < 1750) descens += -150;
		if (i > 1750 && i < 1800) descens += -300;
		if (i > 1810 && i < 2006) descens -= -100;
		line.worldY = descens;

		if ((i / 3) % 2) {
			line.grass.r = 0;
			line.grass.g = 180;
			line.grass.b = 0;
		}
		else {
			line.grass.r = 0;
			line.grass.g = 144;
			line.grass.b = 0;
		}

		if ((i / 3) % 2) {
			line.border.r = 255;
			line.border.g = 255;
			line.border.b = 255;
		}
		else {
			line.border.r = 0;
			line.border.g = 0;
			line.border.b = 248;
		}

		if ((i / 3) % 2) {
			line.road.r = 107;
			line.road.g = 107;
			line.road.b = 107;
		}
		else {
			line.road.r = 105;
			line.road.g = 105;
			line.road.b = 105;
		}

		//if (i>0) line.y = sin(i / 40.0) * 4500;
		map.push_back(line);
	}
	descens = 0;
	for (int i = 0; i < 2700; i++)
	{
		Line line;
		line.worldZ = i*segL;
		if (i<2700 - 300 && i>2700 - 420) line.curve = 15;
		if (i<2700 - 420 && i>2700 - 450) line.curve = 5;
		if (i<2700 - 450 && i>2700 - 550) line.curve = 20;
		if (i<2700 - 600 && i>2700 - 750) line.curve = 3;
		if (i<2700 - 750 && i>2700 - 820) line.curve = -10;
		if (i<2700 - 820 && i>2700 - 950) line.curve = 10;
		if (i<2700 - 950 && i>2700 - 1000) line.curve = -15;
		if (i<2700 - 1100 && i>2700 - 1200) line.curve = -5;
		if (i<2700 - 1200 && i>2700 - 1230) line.curve = -20;

		if (i<2700 - 1300 && i>2700 - 1320) line.curve = -30;
		if (i<2700 - 1320 && i>2700 - 1340) line.curve = 30;

		if (i<2700 - 1400 && i>2700 - 1420) line.curve = 30;
		if (i<2700 - 1420 && i>2700 - 1440) line.curve = -30;

		if (i<2700 - 1550 && i>2700 - 1650) line.curve = -5;

		if (i<2700 - 1650 && i>2700 - 1850) line.curve = 20;
		if (i<2700 - 1850 && i>2700 - 2050) line.curve = -10;

		if (i<2700 - 2100 && i>2700 - 2250) line.curve = 25;

		if (i<2700 - 2300 && i>2700 - 2350) line.curve = -15;
		if (i<2700 - 2350 && i>2700 - 2400) line.curve = 15;




		if (i<2700 - 1050 && i>2700 - 1200) descens += -150;
		if (i<2700 - 1450 && i>2700 - 1550) descens += 200;
		if (i<2700 - 1700 && i>2700 - 1750) descens += 150;
		if (i<2700 - 1750 && i>2700 - 1800) descens += 300;
		if (i<2700 - 1810 && i>2700 - 2006) descens -= 100;

		line.worldY = descens;

		if ((i / 3) % 2) {
			line.grass.r = 0;
			line.grass.g = 180;
			line.grass.b = 0;
		}
		else {
			line.grass.r = 0;
			line.grass.g = 144;
			line.grass.b = 0;
		}

		if ((i / 3) % 2) {
			line.border.r = 255;
			line.border.g = 255;
			line.border.b = 255;
		}
		else {
			line.border.r = 0;
			line.border.g = 0;
			line.border.b = 248;
		}

		if ((i / 3) % 2) {
			line.road.r = 107;
			line.road.g = 107;
			line.road.b = 107;
		}
		else {
			line.road.r = 105;
			line.road.g = 105;
			line.road.b = 105;
		}

		//if (i>0) line.y = sin(i / 40.0) * 4500;
		mapMirror.push_back(line);
	}
}



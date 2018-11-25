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
#include <string>
#include "json.hpp"
#include <iomanip>
#include "Line.h"
#include <fstream>
using namespace std;
using json = nlohmann::json;


enum RoadSprites
{
	WHEELS,
	RIGHTROW,
	LEFTROW,
	RIGHTTURN,
	LEFTTURN,
	FACE,
	NOBEL,
	SHADOW
};

CtrlMap::CtrlMap(bool active) : Module(active)
{
}


CtrlMap::~CtrlMap()
{}

// Load assets
bool CtrlMap::Start()
{
	turnAcceleration = 0;
	skybox = { 0, 0, 320, 200 };
	skyboxMirror = { 6000, 0, 320, 100 };
	sky = { 0, 300, 320, 100 };

	loadRoad();
	
	//generateJSON();

	sizeMap = mapLines.size();
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
	App->textures->Unload(raceSprites);
	App->textures->Unload(skySprites);
	skySprites = nullptr;
	raceSprites = nullptr;
	for (int i = 0; i < spriteVector.size(); ++i)
	{
		delete[] spriteVector[i];
	}
	spriteVector.clear();
	return true;
}

update_status CtrlMap::Update()
{

	speed = App->scene_race->ctrlCar->speed*1.6;
	speedMirror = -speed;
	//UPDATE TURN
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (turnAcceleration < 0) turnAcceleration = 0;
		if (turnAcceleration < 0.0004) turnAcceleration += 0.000003;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
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
	int camH = mapLines[mapPosition].worldY + highWordl;

	mapPositionMirror = worldPositionMirror / segL;
	int camHMirror = mapMirror[mapPositionMirror].worldY + highWordlMirror;

	int maxy = height;
	int maxyMirror = height;
	float x = 0, dx = 0;

	if (mapLines[mapPosition%sizeMap].curve != 0) {
		playerX += -mapLines[mapPosition%sizeMap].curve*0.000005*speed;
		playerXMirror += -mapLines[mapPosition%sizeMap].curve*0.000005*speed;
	}
	App->renderer->Blit(skySprites, 0, -20, &skyboxMirror);
	if(mapPositionMirror == 0)
	{
		int x = 3;
	}
	for (int n = mapPositionMirror; n<mapPositionMirror + 40; n++)
	{
		Line &l = mapMirror[n%sizeMap];
		l.projectMirror(playerXMirror*roadW - x, camHMirror, mapPositionMirror*segL - (n >= sizeMap ? sizeMap*segL : 0));
		x += dx;
		dx += l.curve;

		l.clip = maxyMirror;
		if (l.screenY >= maxyMirror) continue;
		maxyMirror = l.screenY;


		if ((n - 1) < 0)continue;
		Line p = mapMirror[(n - 1) % sizeMap]; //previous line
		if (n  > mapPositionMirror + 3) {
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
	for (int n = mapPositionMirror + 40; n > mapPositionMirror; n--)
	{

		mapMirror[n%sizeMap].drawSpriteMirror(raceSprites);

	}
	App->renderer->Blit(skySprites, 0, 65, &sky);
	App->renderer->Blit(skySprites, 0, sckyBoxY, &skybox);
	//UPDATE NEXT SKYBOX Y
	if (mapLines[mapPosition%sizeMap].worldY < mapLines[(mapPosition + 1) % sizeMap].worldY) {
		sckyBoxY -= 0.1;
	}
	else if (mapLines[mapPosition%sizeMap].worldY > mapLines[(mapPosition + 1) % sizeMap].worldY) {
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
		
		mapLines[n%sizeMap].project(playerX*roadW - x, camH, mapPosition*segL - (n >= sizeMap ? sizeMap*segL : 0));
		Line &l = mapLines[n%sizeMap];
		x += dx;
		dx += l.curve;

		l.clip = maxy;
		if (l.screenY >= maxy) continue;
		maxy = l.screenY;



		Line p = mapLines[(n - 1) % sizeMap]; //previous line

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
	for (int n = mapPosition + 64; n>mapPosition; n--)
	{
		
			mapLines[n%sizeMap].drawSprite(raceSprites);
		
	}



	//UPDATE NEXT SKYBOX X
	if (speed > 0) {
		skyBoxX += mapLines[mapPosition].curve * 0.1f;
		sckyBoxXMirror -= mapLines[mapPosition].curve * 0.1f;
		if (skyBoxX > 5880) skyBoxX = 3200;
		if (sckyBoxXMirror < 0) sckyBoxXMirror = 3200;
		
	}
	if (speed < 0) {
		skyBoxX -= mapLines[mapPosition].curve * 0.1f;
		sckyBoxXMirror += mapLines[mapPosition].curve * 0.1f;
		if (skyBoxX < 0) skyBoxX = 3200;
		if (sckyBoxXMirror > 5880) sckyBoxXMirror = 3200;
	}
	skybox.x = skyBoxX;
	skyboxMirror.x = sckyBoxXMirror;



	if (mapPosition == 0 && lastMapPosition != mapPosition) App->scene_race->nextLap();
	lastMapPosition = mapPosition;
	//App->renderer->Blit(raceSprites, 64, 209, rectFrame, NULL, 1.3);
	if (playerX > 1.6 || playerX < -1.6) {
		if (App->scene_race->ctrlCar->speed > 50)App->scene_race->ctrlCar->speed = 50;
	}
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
	ifstream fileSprite("Files/SpritesRoad.json");
	json jsnSprite;
	fileSprite >> jsnSprite;
	fileSprite.close();
	list<json> spritesJson = jsnSprite;
	spriteVector = vector<SDL_Rect*>(spritesJson.size());
	int j = 0;
	for (list<json>::iterator it = spritesJson.begin(); it != spritesJson.end(); ++it)
	{
		spriteVector[j] = new SDL_Rect();
		spriteVector[j]->x = (*it).at("x");
		spriteVector[j]->y = (*it).at("y");
		spriteVector[j]->w = (*it).at("w");
		spriteVector[j]->h = (*it).at("h");
		++j;
	}

	ifstream ifile("Files/Map.json");
	json jsn;
	ifile >> jsn;
	ifile.close();

	list<json> linesJson = jsn;
	mapLines = vector<Line>(linesJson.size());
	mapMirror = vector<Line>(linesJson.size());
	/*for(int i = 0; i < mapLines.size(); ++i)
	{
		Line line;

		line.curve = linesJson[i].at("curve");
		line.worldY = linesJson[i].at("worldY");
		line.worldZ = linesJson[i].at("worldZ");
		line.road.r = linesJson[i].at("road").at("r");
		line.road.g = linesJson[i].at("road").at("g");
		line.road.b = linesJson[i].at("road").at("b");
		line.grass.r = linesJson[i].at("grass").at("r");
		line.grass.g = linesJson[i].at("grass").at("g");
		line.grass.b = linesJson[i].at("grass").at("b");
		line.border.r = linesJson[i].at("border").at("r");
		line.border.g = linesJson[i].at("border").at("g");
		line.border.b = linesJson[i].at("border").at("b");
		mapLines[i] = line;
		mapMirror[linesJson.size() - 1 - i] = line;
		
	}*/

	/*spriteVector.push_back(new SDL_Rect{ 400, 100, 95, 48 });
	spriteVector.push_back(new SDL_Rect{ 400, 400, 64, 112 });
	spriteVector.push_back(new SDL_Rect{ 475, 400, 64, 112 });
	spriteVector.push_back(new SDL_Rect{ 400, 550, 64, 111 });
	spriteVector.push_back(new SDL_Rect{ 475, 550, 64, 111 });
	spriteVector.push_back(new SDL_Rect{ 550, 400, 104, 64 });
	spriteVector.push_back(new SDL_Rect{ 550, 475, 104, 63 });
	spriteVector.push_back(new SDL_Rect{ 550, 550, 64, 63 });*/
	int i = 0;
	for (list<json>::iterator it = linesJson.begin(); it != linesJson.end(); ++it) {
		Line line;

		line.curve = (*it).at("curve");
		line.worldY = (*it).at("worldY");
		line.worldZ = (*it).at("worldZ");
		line.road.r = (*it).at("road").at("r");
		line.road.g = (*it).at("road").at("g");
		line.road.b = (*it).at("road").at("b");
		line.grass.r = (*it).at("grass").at("r");
		line.grass.g = (*it).at("grass").at("g");
		line.grass.b = (*it).at("grass").at("b");
		line.border.r = (*it).at("border").at("r");
		line.border.g = (*it).at("border").at("g");
		line.border.b = (*it).at("border").at("b");
		line.spriteX = (*it).at("spriteX");
		if((*it).at("sprite") == -1)
		{
			line.sprite = nullptr;
		}else
		{
			line.sprite = spriteVector[(*it).at("sprite")];
		}
		/*int nLin = 16;
		if (i > 20 * nLin && i <= 22 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}
		if (i > 23 * nLin && i <= 27 * nLin) {
			if (i % 16 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[5];
			}
		}
		if (i > 28 * nLin && i <= 30 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}

		if (i > 30 * nLin && i <= 36 * nLin) {
			if (i % 16 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[5];
			}
		}

		if (i > 47 * nLin && i <= 49 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[2];
			}
		}
		if (i > 50 * nLin && i <= 54 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
		}
		if (i > 54 * nLin && i <= 56 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}

		if (i > 56 * nLin && i <= 60 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[0];
			}
		}

		if (i > 64 * nLin && i <= 68 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
		}

		if (i > 74 * nLin && i <= 76 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[2];
			}
		}

		if (i > 78 * nLin && i <= 80 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
			if (i % 8 == 4) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[0];
			}
		}
		
		if (i > 84 * nLin && i <= 86 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[4];
			}
		}
		if (i > 87 * nLin && i <= 89 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
		}
		if (i > 89 * nLin && i <= 91 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[0];
			}
		}

		if (i > 93 * nLin && i <= 95 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[3];
			}
		}
		if (i > 96 * nLin && i <= 98 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[0];
			}
		}
		if (i > 98 * nLin && i <= 100 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
		}
		if (i > 109 * nLin && i <= 111 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}
		if (i > 112 * nLin && i <= 118 * nLin) {
			if (i % 16 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[7];
			}
		}
		if (i > 120 * nLin && i <= 122 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}
		if (i > 123 * nLin && i <= 125 * nLin) {
			if (i % 16 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[7];
			}
		}

		if (i > 165 * nLin && i <= 169 * nLin)
		{
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
			if (i % 8 == 4) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[1];
			}
		}
		if (i > 169 * nLin && i <= 172 * nLin) {
			if (i % 16 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[6];
			}
		}

		if (i > 179 * nLin && i <= 181 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[2];
			}
		}
		if (i > 182 * nLin && i <= 186 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = 1.5f;
				line.sprite = spriteVector[0];
			}
		}
		if (i > 187 * nLin && i <= 191 * nLin) {
			if (i % 8 == 0) {
				line.spriteX = -1.5f;
				line.sprite = spriteVector[0];
			}
		}*/


		line.width = width;
		line.height = height;
		line.heightMirror = heightMirror;
		line.roadW = roadW;
		line.camD = camD;


		mapLines[i] = line;
		line.worldY = -line.worldY;
		line.worldZ = linesJson.size()*segL - line.worldZ;
		mapMirror[mapLines.size() - i -1] = line;
		++i;
	}

	/*
	

	float descens = 0;

	int nLin = 16;
	int desDouble = -1;
	for (int i = 0; i < 222*16; i++)
	{
		Line line;
		line.worldZ = i*segL;
		//////////0--23 cout << line.curve << endl;
		if (i > -1 * nLin && i <= 23 * nLin) line.curve = 0;//4
		if (i > 23 * nLin && i <= 27 * nLin) line.curve = 20;//4
		if (i > 27 * nLin && i <= 30 * nLin) line.curve = 3;//3
		if (i > 30 * nLin && i <= 36 * nLin) line.curve = 25;//8
		if (i > 36 * nLin && i <= 39 * nLin) line.curve = 18;//6
		if (i > 39 * nLin && i <= 44 * nLin) line.curve = 10;//3
		if (i > 44 * nLin && i <= 47 * nLin) line.curve = 3;
		if (i > 47 * nLin && i <= 50 * nLin) line.curve = 0;

		if (i > 50 * nLin && i <= 56 * nLin) line.curve = -20;
		if (i > 56 * nLin && i <= 62 * nLin) line.curve = 20;
		if (i > 62 * nLin && i <= 64 * nLin) line.curve = 0;
		if (i > 64 * nLin && i <= 71 * nLin) line.curve = -8;
		if (i > 71 * nLin && i <= 77 * nLin) line.curve = 0;
		if (i > 77 * nLin && i <= 80 * nLin) line.curve = -30;
		if (i > 80 * nLin && i <= 87 * nLin) line.curve = 0;

		if (i > 87 * nLin && i <= 89 * nLin) line.curve = -35;
		if (i > 89 * nLin && i <= 91 * nLin) line.curve = 35;
		if (i > 91 * nLin && i <= 96 * nLin) line.curve = 0;
		if (i > 96 * nLin && i <= 98 * nLin) line.curve = 35;
		if (i > 98 * nLin && i <= 100 * nLin) line.curve = -35;

		if (i > 100 * nLin && i <= 102 * nLin) line.curve = 0;

		if (i > 102 * nLin && i <= 106 * nLin) line.curve = -5;
		if (i > 106 * nLin && i <= 112 * nLin) line.curve = 0;
		if (i > 112 * nLin && i <= 118 * nLin) line.curve = 20;
		if (i > 118 * nLin && i <= 124 * nLin) line.curve = 28;
		if (i > 124 * nLin && i <= 133 * nLin) line.curve = 20;
		if (i > 133 * nLin && i <= 140 * nLin) line.curve = 0;
		if (i > 140 * nLin && i <= 160 * nLin) line.curve = -12;
		if (i > 160 * nLin && i <= 168 * nLin) line.curve = 0;
		if (i > 168 * nLin && i <= 172 * nLin) line.curve = 50;
		if (i > 172 * nLin && i <= 182 * nLin) line.curve = 0;
		if (i > 182 * nLin && i <= 186 * nLin) line.curve = -30;
		if (i > 186 * nLin && i <= 187 * nLin) line.curve = 0;
		if (i > 187 * nLin && i <= 191 * nLin) line.curve = 30;
		if (i > 191 * nLin && i <= 195 * nLin) line.curve = 0;
		if (i > 195 * nLin && i <= 197 * nLin) line.curve = -20;
		if (i > 197 * nLin && i <= 199 * nLin) line.curve = 20;
		if (i > 199 * nLin && i <= 222 * nLin) line.curve = 0;
		string s = to_string(line.curve);
		s += "\n";
		if(i%16 == 0) printf(&s[0]);

		if (i >= 63 * 16 && i < 71 * 16)
		{
			
			if(desDouble > -90) desDouble += -5;
			descens += desDouble;

		}
		if (i >= 71 * 16 && i < 72 * 16) {
			if (desDouble < -6) desDouble += 5;
			descens += desDouble;
		}
		if (i == 72 * 16)
		{
			desDouble = 0;
		}

		if (i >= 108 * 16 && i < 114 * 16) {
			if (desDouble < 180) desDouble += 5;
			descens += desDouble;
		}
		if (i >= 114 * 16 && i < 117 * 16) {
			if (desDouble > 4) {
				desDouble -= 4;
				descens += desDouble;
			}
		}
		if (i == 117 * 16) desDouble = 0;


		if (i >= 161 * 16 && i < 164 * 16) {
			if (desDouble < 180) desDouble += 5;
			descens += desDouble;
		}
		if (i >= 164 * 16 && i < 167 * 16) {
			if (desDouble > 5) {
				desDouble -= 5;
				descens += desDouble;
			}
		}
		if (i == 167 * 16) desDouble = 0;
		if (i == 168 * 16) {
			desDouble = 0;
		}


		if (i >= 174 * 16 && i < (184 * 16 - 5))
		{

			if (desDouble > -100) desDouble += -3;
			descens += desDouble;

		}
		if (i >= (184 * 16 - 5) && i < 185 * 16) {
			if (desDouble < -6) desDouble += 5;
			descens += desDouble;
		}
		if (i == 185 * 16)
		{
			desDouble = 0;
		}

		if (i >= 185 * 16 && i < 188 * 16) {
			++descens;
		}

		if (i == 200 * 16)
		{
			desDouble = 0;
		}
		
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

		
		mapLines.push_back(line);
	}
	descens = 0;
	for (int i = 0; i < 222 * 16; i++)
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

		mapMirror.push_back(line);
	}*/



	/*for (int i = 0; i < 222 * 16; i++)
	{
		if (i % 400 <= 100) {
			if (i % 6 == 0) {
				mapLines[i].spriteX = -1.5f;
				mapLines[i].sprite = spriteVector[0];
			}
			if (i % 6 == 3)
			{
				mapLines[i].spriteX = 1.5f;
				mapLines[i].sprite = spriteVector[0];
			}
		}

		mapLines[i].width = width;
		mapLines[i].height = height;
		mapLines[i].heightMirror = heightMirror;
		mapLines[i].roadW = roadW;
		mapLines[i].camD = camD;
	}

	for (int i = 0; i < 222 * 16; i++)
	{
		if (i % 400 > 301) {
			if (i % 6 == 0) {
				mapMirror[i].spriteX = -1.5f;
				mapMirror[i].sprite = spriteVector[0];
			}
			if (i % 6 == 3)
			{
				mapMirror[i].spriteX = 1.5f;
				mapMirror[i].sprite = spriteVector[0];
			}
		}

		mapMirror[i].width = width;
		mapMirror[i].height = height;
		mapMirror[i].heightMirror = heightMirror;
		mapMirror[i].roadW = roadW;
		mapMirror[i].camD = camD;
	}*/
}

void CtrlMap::generateJSON()
{
	vector<json> spritesjson = vector<json>(spriteVector.size());
	for (int i = 0; i < spriteVector.size(); ++i)
	{
		json spritejson;
		spritejson["x"] = spriteVector[i]->x;
		spritejson["y"] = spriteVector[i]->y;
		spritejson["w"] = spriteVector[i]->w;
		spritejson["h"] = spriteVector[i]->h;
		spritesjson[i] = spritejson;
	}
	ofstream out("SpritesRoad.json");
	out << std::setw(4) << spritesjson << std::endl;
	out.close();
	vector<json> mapjson = vector<json>(mapLines.size());
	for(int i = 0; i < mapjson.size(); ++i)
	{
		json linejson;
		linejson["curve"] = mapLines[i].curve;
		linejson["worldY"] = mapLines[i].worldY;
		linejson["worldZ"] = mapLines[i].worldZ;
		json grass;
		grass["r"] = mapLines[i].grass.r;
		grass["g"] = mapLines[i].grass.g;
		grass["b"] = mapLines[i].grass.b;
		linejson["grass"] = grass;
		json border;
		border["r"] = mapLines[i].border.r;
		border["g"] = mapLines[i].border.g;
		border["b"] = mapLines[i].border.b;
		linejson["border"] = border;
		json road;
		road["r"] = mapLines[i].road.r;
		road["g"] = mapLines[i].road.g;
		road["b"] = mapLines[i].road.b;
		linejson["road"] = road;
		linejson["spriteX"] = mapLines[i].spriteX;
		if(mapLines[i].sprite == nullptr)
		{
			linejson["sprite"] = -1;
		}else
		{
			for (int j = 0; j < spriteVector.size(); ++j)
			{
				if(spriteVector[j] == mapLines[i].sprite)
				{
					linejson["sprite"] = j;
				}
			}
		}
		mapjson[i] = linejson;
	}
	ofstream o("Map.json");
	o << std::setw(4) << mapjson << std::endl;
	o.close();
}
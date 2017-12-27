/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "vector"
#include <iostream>
using namespace std;
//Screen dimension constants
const int SCREEN_WIDTH = 1300;
const int SCREEN_HEIGHT = 700;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};


//customs

int getPosition(int position);
void printLine(int i, int &lineToPint, const int &z, SDL_Rect &topRoad, SDL_Rect &bottomRoad);
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Road animation
const int WALKING_ANIMATION_FRAMES = 2;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;

//Car
LTexture car;
SDL_Rect carRect;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0x00, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load sprite sheet texture
	if (!gSpriteSheetTexture.loadFromFile("Sprites/Sprites.png"))
	{
		printf("Failed to load walking animation texture!\n");
		success = false;
	}
	else
	{
		//Set sprite clips
		/*gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 1277;
		gSpriteClips[0].h = 237;

		gSpriteClips[1].x = 0;
		gSpriteClips[1].y = 238;
		gSpriteClips[1].w = 1277;
		gSpriteClips[1].h = 238;**/

		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 477;
		gSpriteClips[0].w = 1277;
		gSpriteClips[0].h = 237;

		gSpriteClips[1].x = 0;
		gSpriteClips[1].y = 715;
		gSpriteClips[1].w = 1277;
		gSpriteClips[1].h = 237;
	}
	if (!car.loadFromFile("Sprites/dummyCar.png"))
	{
		printf("Failed to load walking animation texture!\n");
		success = false;
	}
	else
	{
	}
	return success;
}

void close()
{
	//Free loaded images
	gSpriteSheetTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void printLine(int i, int &lineToPint, const int &z,  SDL_Rect &topRoad,  SDL_Rect &bottomRoad)
{
	if ((z % 6000) < 800) gSpriteSheetTexture.render((SCREEN_WIDTH - (&gSpriteClips[0])->w) / 2, lineToPint, &topRoad);
	else gSpriteSheetTexture.render((SCREEN_WIDTH - (&gSpriteClips[1])->w) / 2, lineToPint, &bottomRoad);
}

int getHight(int position) {

	if (position < 80000) return 0;
	if (position < 150000) return 1;
	if (position < 160000) return 0;
	if (position < 250000) return -1;
	if (position < 300000) return -1;
	if (position < 400000) return 0;
	if (position < 500000) return 1;
	if (position < 700000) return -1;
	if (position < 800000) return 0;
	if (position < 900000) return 1;
	if (position < 1000000) return -1;
	if (position < 1100000) return 1;
	if (position < 1200000) return -1;
}
int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Current animation frame
			int frame = 0;

			//Max distance to paint
			int zfar = 22000;
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0x66, 0x66, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render current frame
				

				//gSpriteSheetTexture.render((SCREEN_WIDTH - (&gSpriteClips[1])->w) / 2, (SCREEN_HEIGHT - (&gSpriteClips[1])->h) , (&gSpriteClips[1]));

				vector<int> distLine = { 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465, 496, 528, 561, 595, 630, 666, 703, 741, 780, 820, 861, 903, 946, 990, 1035, 1081, 1128, 1176, 1225, 1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770, 1830, 1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415, 2485, 2556, 2628, 2701, 2775, 2850, 2926, 3003, 3081, 3160, 3240, 3321, 3403, 3486, 3570, 3655, 3741, 3828, 3916, 4005, 4095, 4186, 4278, 4371, 4465, 4560, 4656, 4753, 4851, 4950, 5050, 5151, 5253, 5356, 5460, 5565, 5671, 5778, 5886, 5995, 6105, 6216, 6328, 6441, 6555, 6670, 6786, 6903, 7021, 7140, 7260, 7381, 7503, 7626, 7750, 7875, 8001, 8128, 8256, 8385, 8515, 8646, 8778, 8911, 9045, 9180, 9316, 9453, 9591, 9730, 9870, 10011, 10153, 10296, 10440, 10585, 10731, 10878, 11026, 11175, 11325, 11476, 11628, 11781, 11935, 12090, 12246, 12403, 12561, 12720, 12880, 13041, 13203, 13366, 13530, 13695, 13861, 14028, 14196, 14365, 14535, 14706, 14878, 15051, 15225, 15400, 15576, 15753, 15931, 16110, 16290, 16471, 16653, 16836, 17020, 17205, 17391, 17578, 17766, 17955, 18145, 18336, 18528, 18721, 18915, 19110, 19306, 19503, 19701, 19900, 20100, 20301, 20503, 20706, 20910, 21115, 21321, 21528, 21736, 21945, 22155, 22366, 22578, 22791, 23005, 23220, 23436, 23653, 23871, 24090, 24310, 24531, 24753, 24976, 25200, 25425, 25651, 25878, 26106, 26335, 26565, 26796, 27028, 27261, 27495, 27730, 27966, 28203 };
				SDL_Rect topRoad = gSpriteClips[0];
				SDL_Rect bottomRoad = gSpriteClips[1];
				int thisDistance = frame;
				int lineToPaint = SCREEN_HEIGHT;
				int lineToGetTop = (&gSpriteClips[0])->y + (&gSpriteClips[0])->h;
				int lineToGetBottom = (&gSpriteClips[1])->y + (&gSpriteClips[1])->h;
				int myPosition = distLine[0] + frame;
				int myHight = getHight(myPosition);

				for (int i = 0; i < (&gSpriteClips[1])->h; ++i)
				{
					thisDistance = distLine[i] + frame;
					bottomRoad.y = lineToGetBottom;
					topRoad.y = lineToGetTop;
					bottomRoad.h = 1;
					topRoad.h = 1;

					if (distLine[i] < zfar) {

						//The order is important, can't be refactor becouse the result is diferent.
						if (myHight == 0 && getHight(thisDistance) == 0) {
							if (zfar < 22000) zfar += 200;
							if (zfar > 22000) zfar -= 200;
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
						}
						else if (myHight == 1 && getHight(thisDistance) == 1) {
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 2) == 0)
							{
								printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
								if (zfar < 28000) zfar += 2;
								--lineToPaint;
							}
						}
						else if (myHight == -1 && getHight(thisDistance) == -1) {
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 3) == 0)
							{
								++lineToPaint;
								if (zfar < 28000) zfar += 2;
							}
						}
						else if (myHight == 0 && getHight(thisDistance) == 1) {
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 2) == 0)
							{
								printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
								--lineToPaint;
								if (zfar < 28000) zfar += 2;
							}
						}
						else if (myHight == 0 && getHight(thisDistance) == -1) {
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 3) == 0)
							{
								++lineToPaint;
								if (zfar < 28000) zfar += 2;
							}
						}
						else if (myHight == 1 && getHight(thisDistance) == 0) {
							
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 2) == 0)if (zfar > 0) zfar -= 5;
						}
						else if (myHight == -1 && getHight(thisDistance) == 0) {
							printLine(i, lineToPaint, thisDistance, topRoad, bottomRoad);
							--lineToPaint;
							--lineToGetBottom;
							--lineToGetTop;
							if ((i % 3) == 0)if (zfar > 0) zfar -= 5;
						}
					}
				}



				
				car.render(SCREEN_WIDTH / 2 - car.getWidth()  / 2, SCREEN_HEIGHT - car.getHeight(), nullptr);

				//Update screen
				SDL_RenderPresent(gRenderer);

				//Go to next frame
				frame+=01200;

				//Cycle animation
				/*if (frame / 2 >= WALKING_ANIMATION_FRAMES)
				{
					frame = 0;
				}*/
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
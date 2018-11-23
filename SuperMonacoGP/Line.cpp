#include "Line.h"
#include "Application.h"
#include "ModuleRender.h"


Line::Line()
{
	spriteX = curve = worldX = worldY = worldZ = 0;
}


Line::~Line()
{
}

void Line::project(int camX, int camY, int camZ)
{
	scale = camD / (worldZ - camZ);
	screenX = (1 + scale * (worldX - camX)) * width / 2;
	screenY = (1 - scale * (worldY - camY)) * height / 2;
	screenW = scale * roadW  * width / 2;
}

void Line::projectMirror(int camX, int camY, int camZ)
{
	scale = camD / (worldZ - camZ);
	screenX = (1 + scale * (worldX - camX)) * width / 2;
	screenY = (1 - scale * (worldY - camY)) * heightMirror / 2;
	screenW = scale * roadW  * width / 2;
}

void Line::drawSprite(SDL_Texture* tex)
{
	if (sprite != nullptr)
	{
		SDL_Rect rect = *sprite;
		int w = rect.w;
		int h = rect.h;

		float destX = screenX + scale * spriteX * width / 2;
		float destY = screenY + 4;
		float destW = w * screenW / 230;
		float destH = h * screenW / 230;

		destX += destW * spriteX; //offsetX
		destY += destH * (-1);    //offsetY

		float clipH = destY + destH - clip;
		if (clipH < 0) clipH = 0;

		destX = screenX + (screenW * spriteX);
		if (clipH >= destH) return;

		float spriteH = (int)(h - h * clipH / destH);
		int spriteScaleH = (int)(spriteH*(destH / h));
		int spriteScaleW = (int)(rect.w*(destW / w));
		App->renderer->BlitScaled(tex, (int)(destX - spriteScaleW / 2), (int)destY + ((SCREEN_HEIGHT / 2)*SCREEN_SIZE), &rect, 0.f, 1.f, spriteScaleW, spriteScaleH);
	}
}

	
	
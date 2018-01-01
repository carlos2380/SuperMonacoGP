#include "TextFont.h"
#include "Application.h"
#include "ModuleRender.h"

TextFont::TextFont()
{
}

TextFont::TextFont(SDL_Texture* sprites, const string& letters, const int x, const int y, const int w, const int h)
{
	letterWidth = w;
	letterHeight = h;
	this->sprites = sprites;
	lettersRect = vector<SDL_Rect*>(90);
	int posX = x;
	for (int i = 0; i < letters.length(); ++i)
	{
		SDL_Rect rect = { posX, y, w, h };
		//Start in space from table ascii
		lettersRect[letters[i] - ' '] = new SDL_Rect(rect);
		posX += w;
	}
	SDL_Rect rect = { 0, 0, 0, 0 };
	lettersRect[0] = new SDL_Rect(rect);
}


TextFont::~TextFont()
{
	for (int i = 0; i < lettersRect.size(); ++i)
	{
		if(lettersRect[i] != NULL)
		{
			delete lettersRect[i];
		}
	}
}

void TextFont::print(const int x, const int y, const string& text) const
{
	int posX = x;
	for (int i = 0; i < text.size(); ++i)
	{
		App->renderer->Blit(sprites, posX, y, lettersRect[text[i] - ' ']);
		posX += letterWidth;
	}
}



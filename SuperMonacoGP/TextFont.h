#ifndef __TEXTFONT_H__
#define __TEXTFONT_H__

#include "Globals.h"
#include <string>
#include <vector>

using namespace std;

struct SDL_Texture;
class Application;
class TextFont
{
public:
	TextFont();
	//x and y first position(up/left) of all letters, w and h is the width and height of letters
	TextFont(SDL_Texture* sprites, const string &letters, const int x, const int y, const int w, const int h);
	~TextFont();
	//x and y first position(up/left) to print
	void print(const int x, const int y, const string &text) const;

public:
	SDL_Texture* sprites = nullptr;

private:
	int letterWidth;
	int letterHeight;
	vector<SDL_Rect*> lettersRect;
};

#endif

#pragma once

#include "glyph.h"

class Char :public Glyph
{
public:
	Char(void);
	Char(int size, HFONT font, wchar_t symbol, COLORREF rgb, bool state);
	~Char(void);

	virtual int getHeight();
	virtual int getWidth(); 
	virtual HDC draw(HDC hdc, int x0, int y0, HFONT font);
	virtual wchar_t getSymbol();

private:
	void computeRect();
	int height, width, loupe, size;
	bool state;
	wchar_t symbol;
	HFONT font;
	COLORREF rgb;
};


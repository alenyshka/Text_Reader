#pragma once

#include "Glyph.h"

class Bitmap : public Glyph
{
	private:
		int height, width, loupe;
	public:
		Bitmap(void);
		~Bitmap(void);
		
		virtual int getHeight();
		virtual int getWidth();
		virtual HDC draw(HDC hdc, int x0, int y0);
/*		virtual VOID setSymbol(char c);
		virtual HDC setSize(HDC hdc, int size);
		virtual HDC setColor(HDC hdc, COLORREF rgb);
		virtual HDC setFont(HDC hdc, HFONT font);*/
};


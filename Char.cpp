#include "stdafx.h"
#include "Char.h"

#define SIZE 20
#define COLOR 255

Char::~Char(void)
{
}

Char::Char(int size, HFONT font, wchar_t symbol, COLORREF rgb, bool state)
{
	this->size = size;
	this->font = font;
	this->symbol = symbol;
	this->rgb = rgb;
	this->state = state;
	computeRect();
}

HDC Char::draw(HDC hdc, int x0, int y0, HFONT font)
{
	if (this->getWidth() != -1)
	{
		SetTextColor(hdc, this->rgb);
		SetBkColor(hdc, RGB(COLOR, COLOR, COLOR));
		TextOut(hdc, x0, y0, (wchar_t *)&symbol, 1);
	}
	return hdc;
}

int Char::getWidth()
{
	return width;
}
wchar_t Char::getSymbol()
{
	return symbol;
}

int Char::getHeight()
{
	if (symbol == L'\n')
	{
		return SIZE;
	}
	return height;
}

void Char::computeRect()
{
	HDC hDC = GetDC(NULL) ;
	HGDIOBJ oldfont = SelectObject(hDC, this->font) ;
	RECT rect ;
	DrawText(hDC, &(this->symbol), 1, &rect, DT_CALCRECT|DT_NOPREFIX) ;
	SelectObject(hDC, oldfont) ;
	ReleaseDC(NULL, hDC);

	if (this->size != -1)
	{
		width = rect.right-rect.left;
		height =rect.bottom - rect.top+1;
	}
	else
	{ 
		width = -1;
		height = size;
	}
}

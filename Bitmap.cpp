#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap(void)
{
}

Bitmap::~Bitmap(void)
{
}
		
int Bitmap::getHeight()
{
	return this->height;
}

int Bitmap::getWidth()
{
	return this->width;
}

HDC Bitmap::draw(HDC hdc, int x0, int y0)
{
	return hdc;
};

wchar_t Bitmap::getSymbol(){
	return L'';
}

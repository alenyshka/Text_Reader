#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap(void)
{
}

Bitmap::~Bitmap(void)
{
}
		
int Bitmap::getHeight(){
	return this->height;
}

int Bitmap::getWidth(){
	return this->width;
}

HDC Bitmap::draw(HDC hdc, int x0, int y0){
	return hdc;
};

wchar_t Bitmap::getSymbol(){
	return L'';
}
/*
VOID setSymbol(char c){
	return;
}

HDC setSize(HDC hdc, int size){
	return hdc;
}

HDC setColor(HDC hdc, COLORREF rgb){
	return hdc;
}

HDC setFont(HDC hdc, HFONT font){
	return hdc;
}*/
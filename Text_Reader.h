#pragma once

#include "resource.h"


void key_up(HWND hWnd);
void key_down(HWND hWnd);
int backspace(HWND hWnd);
int delete_glyph(HWND hWnd);
void repaintWindow(HWND hWnd);
int	getWindowWidth(HWND hWnd);
int	getWindowHeight(HWND hWnd);
HWND CreateTextEditorWindow(HWND, RECT);
wchar_t GetTextFromList(wchar_t *text, int size);
void GetListFromText(wchar_t *text, int size);
bool keydown(int key);
void insert_tab();

VOID print_glyphs(HWND hWnd);
void ToCreateCaret(HWND hWnd);
void Save(HWND hWnd);
int Open(HWND hWnd);
HWND New(HWND hWnd, RECT rect);
void Print(HWND h, int x1, int x2, int y1, int y2);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


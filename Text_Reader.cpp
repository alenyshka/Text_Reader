// Text_Reader.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Text_Reader.h"
#include <CommDlg.h>
#include <stdio.h>
#include <string>
#include <list>
#include "Text_Reader.h"
#include "Glyph.h"
#include "Bitmap.h"
#include "Char.h"
#include <vector>
#include "Commctrl.h"
#include <fstream>
#include <iostream>
#include <winddi.h>
#include <fcntl.h> 



#pragma comment (lib, "comctl32.lib")

#define MAX_LOADSTRING 100
#define CORRECT_CARET_POSITION 45
#define WIDTH_SCROOL 20
#define SIZE 20


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


BOOL OpenFile(HWND, OPENFILENAME);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

VOID print_glyphs(HWND hWnd);
void ToCreateCaret(HWND hWnd);
void Save(HWND hWnd);
int Open(HWND hWnd);
HWND New(HWND hWnd, RECT rect);
void Print(HWND h, int x1, int x2, int y1, int y2);


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
static HIMAGELIST g_hImageList = NULL;			//for toolbar
int	wmId, wmEvent, heightPositionCarret = 0, count_string = 0, widthPositionCarret = 0, widthWindow, heightWindow;
HDC static hdc;

static OPENFILENAME ofn;
static HFONT font = CreateFont(SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
static char fullpath[256], filename[256], dir[256];

RECT rect;

static DOCINFO di;
PRINTDLG pd;

std::list<Glyph*> glyph_list;
std::list<Glyph*>::iterator CarriagePosition;

PAINTSTRUCT ps;
COLORREF color = RGB(0, 0, 0);

int WndWidth, WndHeight;
HWND ed, scr;
static int nScrollPos;
SCROLLINFO si = { sizeof(SCROLLINFO), SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS, 1, 20, 0, 0, 0 };
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEXT_READER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEXT_READER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//



HWND CreateSimpleToolbar(HWND hWndParent)
{
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 5;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0, hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_FILENEW, ImageListID), ID_FILE_NEW, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"New" },
		{ MAKELONG(STD_FILEOPEN, ImageListID), ID_FILE_OPEN, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"Open" },
		{ MAKELONG(STD_FILESAVE, ImageListID), ID_FILE_SAVE, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"Save" },
		{ MAKELONG(STD_PRINT, ImageListID), ID_FILE_PRINT, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"Print" },
		{ MAKELONG(STD_HELP, ImageListID), IDM_ABOUT, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"About " }
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}

HWND CreateAVerticalScrollBar(HWND hwndParent, int sbHeight)
{
	RECT rect;

	// Get the dimensions of the parent window's client area;
	if (!GetClientRect(hwndParent, &rect))
		return NULL;
	HWND scroll = CreateWindowEx(
		0,                      // no extended styles 
		L"SCROLLBAR",           // scroll bar control class 
		(PTSTR)NULL,           // no window text 
		WS_CHILD | WS_VISIBLE   // window styles  
		| SBS_VERT,         // vertical scroll bar style 
		rect.right - WIDTH_SCROOL,              // horizontal position 
		CORRECT_CARET_POSITION,  // vertical position 
		sbHeight,             // width of the scroll bar 
		rect.bottom - rect.top - CORRECT_CARET_POSITION,               // height of the scroll bar
		hwndParent,             // handle to main window 
		(HMENU)NULL,           // no menu 
		hInst,                // instance owning this window 
		(PVOID)NULL            // pointer not needed 
		);
	// Create the scroll bar.
	// SetScrollRange(scroll, SB_VERT, 1,	20,	TRUE);
	return scroll;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEXT_READER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEXT_READER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;
	static int s1Pos, s1Min, s1Max;
	//HWND scroll = NULL;
	
	switch (message)
	{
		case WM_CREATE:
		{
			CreateTextEditorWindow(hWnd, rect);
			CreateSimpleToolbar(hWnd);
			CreateAVerticalScrollBar(hWnd, WIDTH_SCROOL);
			break;
		}
		case WM_VSCROLL:
		{
					   
			int nScrollCode = (int)LOWORD(wParam);
			int nPos = (short int)HIWORD(wParam);

			GetScrollInfo(hWnd, SB_VERT, &si);
			int nNewPos = si.nPos;
			//SetScrollRange(hWnd, SB_VERT, 1, 20, FALSE);
			switch (nScrollCode)
			{
				case SB_TOP:
				{
					nNewPos = 1;
					break;
				}
				case SB_BOTTOM:
				{
					nNewPos = 20;
					break;
				}
				case SB_LINEUP:
				{
					nNewPos -= 1;
					//SetScrollPos(hWnd, SB_VERT, si.nPos, TRUE);
					break;
				}
				case SB_LINEDOWN:
				{
					nNewPos += 1;
					//SetScrollPos(hWnd, SB_VERT, si.nPos, TRUE);
					break;
				}
					// Include code that checks for other values of nScrollCode.
					// ...
				case SB_THUMBPOSITION:
				{
					nNewPos = nPos + si.nMin; // Adding si.nMin is the workaround.
					break;
				}	
			}
			si.fMask = SIF_POS;
			si.nPos = nNewPos;
			int result = SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
			//int result = SetScrollPos(hWnd, SB_VERT, si.nPos, TRUE);
			if (result == 0)
				break;
		}
		return TRUE;

	

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				//FILE
			case ID_FILE_NEW:
				New(hWnd, rect);
				break;
			case ID_FILE_OPEN:	
				Open(hWnd);
				break;
			case ID_FILE_SAVE:
				Save(hWnd);
				break;

			case ID_FILE_PRINT:
			{
				Print(hWnd, rect.left, rect.right, rect.top, rect.bottom);
				ShowCaret(hWnd);
				break;
			}
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_SYSKEYDOWN:
			break;
		case WM_SYSCHAR:
			break;
		case WM_CHAR:
			switch (wParam)
			{

			case VK_RIGHT: 
			case VK_DELETE: 
			case VK_BACK: 
			case VK_RETURN: 
				break;
			default:
				glyph_list.insert(CarriagePosition, new Char(SIZE, font, wParam, color, false));
				InvalidateRect(hWnd, NULL, true);
				break;
			}
			break;

		case WM_KEYDOWN:	// Сообщение от клавиатуры
			switch (wParam)
			{
			case VK_LEFT:				//left
			{
				if (CarriagePosition != ++glyph_list.begin())
				{
					--CarriagePosition;
				}
				InvalidateRect(hWnd, NULL, true);
				break;
			}
			case VK_RIGHT:				//right
			{
				if (CarriagePosition != glyph_list.end())
				{
					++CarriagePosition;
				}
				InvalidateRect(hWnd, NULL, true);
				break;
			}
			case VK_RETURN:				//enter
			{
				glyph_list.insert(CarriagePosition, new Char(-1, font, L'\n', color, true));
				InvalidateRect(hWnd, NULL, true);
				break;
			}
				
			case VK_UP:					//up
			{
				key_up(hWnd);
				break;
			}
			case VK_DOWN:				//dowm
			{
				key_down(hWnd);
				break;
			}
			case VK_LSHIFT:				//shift+left
				break;
			case VK_RSHIFT:				//shift+right
				break;
			case VK_DELETE:				// delete
			{
				int result_delete;
				result_delete = delete_glyph(hWnd);
				if (result_delete == 0)
				{
					++CarriagePosition;
					InvalidateRect(hWnd, NULL, true);
				}
				break;
			}
			case VK_BACK:				// backspace
			{
				int result_bacspace;
				result_bacspace = backspace(hWnd);
				if (result_bacspace == 0)
				{
					++CarriagePosition;
					InvalidateRect(hWnd, NULL, true);
				}
				break;
			}
			case VK_TAB:				//tab
			{
				insert_tab();
				InvalidateRect(hWnd, NULL, true);
				break;
			}
			
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		break;

	case WM_SIZING:
		repaintWindow(hWnd);
		InvalidateRect(hWnd, NULL, true);
		break;
	case SW_MAXIMIZE:
		repaintWindow(hWnd);
		break;
	case WM_PAINT:
		print_glyphs(hWnd);
		break;
	case WM_SETFOCUS:
		ToCreateCaret(hWnd);
		break;
	case WM_KILLFOCUS:
		HideCaret(hWnd);
		DestroyCaret();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
	
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID print_glyphs(HWND hWnd)
{
	HDC hDC = BeginPaint(hWnd, &ps);
	CarriagePosition--;

	int summWidth = 0, height = 0, maxHeight = 0; count_string = 0; widthPositionCarret = 0;
	std::list<Glyph*>::iterator itemList;
	for (itemList = ++glyph_list.begin(); itemList != glyph_list.end(); itemList++)
	{
		Glyph * symbol = *itemList;
		if (((summWidth + symbol->getWidth()) > widthWindow) || (symbol->getWidth() == -1))
		{
			summWidth = 1;
			count_string++;
			if (maxHeight == 0)
				height += symbol->getHeight();
			else
				height += maxHeight;
			maxHeight = 0;
			widthPositionCarret = 0;
		}
		
		symbol->draw(hDC, summWidth, 45 + height, font);
		
		if (symbol->getWidth() > 0)
			summWidth += symbol->getWidth();
		if (maxHeight < symbol->getHeight())
			maxHeight = symbol->getHeight();
		if (itemList == CarriagePosition)
		{
			heightPositionCarret = count_string;
			widthPositionCarret = summWidth;
			SetCaretPos(summWidth, CORRECT_CARET_POSITION + height);
		}
	}
	CarriagePosition++;
	//	BitBlt(hDC, 0, 0, widthWindow, heightWindow, hCompDC, 0, 0, SRCCOPY);
	DeleteDC(hDC);
	EndPaint(hWnd, &ps);
}

void key_up(HWND hWnd)
{
	if (heightPositionCarret != 0)
	{
		int summWidth = 0, height = 0, maxHeight = 0, temp_count_string = 0, countShift = 0;
		std::list<Glyph*>::iterator itemList = glyph_list.begin();	
		while (!(++itemList == glyph_list.end()) && (itemList != CarriagePosition)) //find position and find out countShift
		{
			Glyph * symbol = *itemList;
			countShift++;
			if (symbol->getWidth() < 0)
			{
				temp_count_string++;
				countShift = 0;
			}

			if (maxHeight < symbol->getHeight())
				maxHeight = symbol->getHeight();
		}
		//make carretposition on temo_count_string-1
		int count_string = 0;
		itemList = glyph_list.begin();
		while (!(++itemList == glyph_list.end()) && (count_string < temp_count_string - 1)) //find position and find out countShift
		{
			Glyph * symbol = *itemList;
			if (symbol->getWidth() < 0)
			{
				count_string++;
			}
		}
		//переместить на нужную позицию
		CarriagePosition = itemList;
		Glyph * symbol = *itemList;
		while ((countShift != 0) && (symbol->getWidth() > 0))
		{
			Glyph * symbol = *itemList;
			itemList++;
			CarriagePosition++;
			countShift--;
		}
		
	}
	InvalidateRect(hWnd, NULL, true);
}

void key_down(HWND hWnd)
{
	if ((count_string == heightPositionCarret) && (glyph_list.end() != CarriagePosition))
	{
		CarriagePosition++;
		InvalidateRect(hWnd, NULL, true);
		return;
	}
	if (glyph_list.end() == CarriagePosition)
		return;

	int summWidth = 0, tempCountString = 0, countShift = 0;
	Glyph* symbol;
	std::list<Glyph*>::iterator itemList = glyph_list.begin();
	while (++itemList != CarriagePosition)
	{
		symbol = *itemList;
		summWidth += symbol->getWidth();
		countShift++;
		if (((summWidth + symbol->getWidth() > widthWindow) || (symbol->getWidth() == -1) || (symbol->getWidth() == 0)))
		{
			countShift = 0;
			summWidth = 0;
			tempCountString++;
		}
	}
	symbol = *itemList;
	while (symbol->getWidth() != -1)
	{
		itemList++;
		symbol = *itemList;	
	}
	CarriagePosition = itemList;
	while ((countShift != -1) && !(++itemList == glyph_list.end()))// && ((symbol->getWidth() == -1) || (symbol->getWidth() == 0)))
	{
		symbol = *itemList;
		countShift--;
		CarriagePosition++;
	}
	InvalidateRect(hWnd, NULL, true);
}

int delete_glyph(HWND hWnd)
{
	int result = -1;
	std::list<Glyph*>::iterator itemList = glyph_list.begin();
	while (!(++itemList == glyph_list.end()))
	{
		Glyph * symbol = *itemList;
		if (itemList == CarriagePosition)
		{
			--CarriagePosition;
			glyph_list.remove(symbol);
			return  0;
		}
	}
	return result;
}

int backspace(HWND hWnd)
{
	int result = -1;
	std::list<Glyph*>::iterator itemList = glyph_list.begin();
	if (glyph_list.size() != 1)
	{
		--CarriagePosition;
	}
	while (!(++itemList == glyph_list.end()))
	{
		Glyph * symbol = *itemList;
		if (itemList == CarriagePosition)
		{
			--CarriagePosition;
			glyph_list.remove(symbol);
			return  0;
		}
	}
	if ((itemList == glyph_list.end()) && (glyph_list.size() != 1))
	{
		--itemList;
		Glyph * symbol = *itemList;
		--CarriagePosition;
		glyph_list.remove(symbol);
		return  0;
	}
	return result;
}

void ToCreateCaret(HWND hWnd)
{
	CreateCaret(hWnd, (HBITMAP)0, 2, SIZE + 2);
	SetCaretPos(0, CORRECT_CARET_POSITION);
	ShowCaret(hWnd);
}

void repaintWindow(HWND hWnd)
{
	GetClientRect(hWnd, &rect);
	heightWindow = rect.bottom - rect.top;
	widthWindow = rect.right - rect.left;
	print_glyphs(hWnd);
}

HFONT setFont(LPCWSTR font, int size){
	 HFONT hFont = CreateFont(size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, font);
	 SelectObject(hdc, hFont);
	 return hFont;
}

int getWindowWidth(HWND hWnd)
{
	GetClientRect(hWnd, &rect);
	return rect.right - rect.left;
}

int getWindowHeight(HWND hWnd)
{
	GetClientRect(hWnd, &rect);
	//SetRect(&rect, 0, 16, 100, 100);
	return rect.bottom - rect.top;
}

HWND CreateTextEditorWindow(HWND hWnd, RECT rect)
{
	font = setFont(L"Times New Roman", SIZE);
	CarriagePosition = glyph_list.begin();
	glyph_list.insert(CarriagePosition, new Char(-1, font, 0x0D, color, false));
	ShowWindow(hWnd, SW_MAXIMIZE);
	HDC hdc = GetDC(hWnd);
	GetClientRect(hWnd, &rect);
	widthWindow = rect.right - rect.left;
	heightWindow = rect.bottom - rect.top;
	return hWnd;
}

HWND New(HWND hWnd, RECT rect)
{
	font = setFont(L"Times New Roman", SIZE);
	glyph_list.clear();
	SetCaretPos(1, CORRECT_CARET_POSITION);
	glyph_list.insert(CarriagePosition, new Char(-1, font, 0x0D, color, false));
	InvalidateRect(hWnd, NULL, true);
	return hWnd;
}

void Save(HWND hWnd)
{
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = __TEXT("Текстовый файл (*.txt)\0*.txt\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = (LPWSTR)fullpath;
	ofn.nMaxFile = sizeof(fullpath);
	ofn.lpstrFileTitle = (LPWSTR)filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrInitialDir = (LPCWSTR)dir;
	ofn.lpstrTitle = __TEXT("Save file as...");
	ofn.lpstrDefExt = __TEXT("txt");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
	if (GetSaveFileName(&ofn))
	{
		int size = glyph_list.size() - 1;
		wchar_t *text = new wchar_t[size]{NULL};
		//wchar_t *text;
		//text = (wchar_t *)calloc(size, sizeof(wchar_t));
		*text = GetTextFromList(text, size);
		FILE* outFile;
		_wfopen_s(&outFile, (LPWSTR)fullpath, L"w+,ccs=UTF-16LE");
		fwrite((char*)text, sizeof(wchar_t), size, outFile);
		fclose(outFile);
	}
}

wchar_t GetTextFromList(wchar_t *text, int size)
{
	std::list<Glyph*>::iterator itemList = glyph_list.begin();
	int i = 0;
	while (!(++itemList == glyph_list.end()))
	{
		Glyph * glyph = *itemList;
		text[i++] = glyph->getSymbol();//&&SO_GLYPHINDEX_TEXTOUT;//		(*((Char*)(symbol))).symbol	//	wchar_t SO_GLYPHINDEX_TEXTOUT
	}
	return *text;
}

void GetListFromText(wchar_t *text, int size)
{
	glyph_list.clear();
	bool flag = true;
	std::list<Glyph*>::iterator itemList = glyph_list.begin();
	int i = 0;
	Glyph * symbol = new Char(SIZE, font, L' ', color, false);
	glyph_list.insert(CarriagePosition, symbol);
	while (i < size)
	{
		if (text[i] == L'\n')
		{
			glyph_list.insert(CarriagePosition, new Char(-1, font, L'\n', color, true));	
			if (flag == true)
			{
				size -= 2;
				flag = false;
			}
			else
				size--;
		}
		else
		{
			glyph_list.insert(CarriagePosition, new Char(SIZE, font, text[i], color, false));
		}
		i++;
	}
}

DWORD nBytesRead;
CHAR *lpBuffer;

int Open(HWND hWnd)
{
	New(hWnd, rect);
	GetClientRect(hWnd, &rect);
	
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = __TEXT("Текстовый файл (*.txt)\0*.txt\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = (LPWSTR)fullpath;
	ofn.nMaxFile = sizeof(fullpath);
	ofn.lpstrFileTitle = (LPWSTR)filename;
	ofn.nMaxFileTitle = sizeof(filename);
	ofn.lpstrInitialDir = (LPCWSTR)dir;
	ofn.lpstrTitle = __TEXT("Open...");
	ofn.lpstrDefExt = __TEXT("txt");//расширение по умолчанию 
	if (GetOpenFileName(&ofn))
	{
		HANDLE hFile = CreateFile((LPWSTR)fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING,//Открытие файла 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, __TEXT("Ошибка открытия файла"), __TEXT("Ошибка! "), MB_OK | MB_ICONSTOP);
			return 0;
		}
		int SizeFile = GetFileSize(hFile, NULL);//Получить размер файла 
		CloseHandle(hFile);

		wchar_t *text = new wchar_t[SizeFile /sizeof(wchar_t)]{L''};
		wchar_t line;
		int i = 0;
		FILE* outFile;
		if (_wfopen_s(&outFile, (LPWSTR)fullpath, L"r+,ccs=UTF-16LE") == 0)
		{
			fread_s(text, SizeFile, sizeof(wchar_t), SizeFile / sizeof(wchar_t), outFile);
			fclose(outFile);
		}
		glyph_list.clear();
		GetListFromText(text, SizeFile / sizeof(wchar_t));
	}
	InvalidateRect(hWnd, NULL, true);
	return 0;
}
HBITMAP hbmMem;
HANDLE hOld;

void Print(HWND h, int x1, int x2, int y1, int y2)
{
	int scale = 1;
	double coef = 0.5;
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = h;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;
	POINTS point;

	if (PrintDlg(&pd) == TRUE)
	{

		int Rx = GetDeviceCaps(pd.hDC, HORZRES);
		int Ry = GetDeviceCaps(pd.hDC, VERTRES);

		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = L"Print text";
		di.fwType = NULL;
		di.lpszDatatype = NULL;
		di.lpszOutput = NULL;
		StartDoc(pd.hDC, &di);

		StartPage(pd.hDC);
		GetClientRect(h, &rect);
		HDC hdc = GetDC(h);
		HideCaret(h);
		StretchBlt(pd.hDC, 0, 0, Rx, Ry, hdc, 0, 43, rect.right - 500, rect.bottom, SRCCOPY);

		EndPage(pd.hDC);
		EndDoc(pd.hDC);
		DeleteDC(hdc);
		DeleteDC(pd.hDC);
		
	}
}


bool keydown(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

void insert_tab()
{
	glyph_list.insert(CarriagePosition, new Char(SIZE, font, L' ', color, true));
	glyph_list.insert(CarriagePosition, new Char(SIZE, font, L' ', color, true));
	glyph_list.insert(CarriagePosition, new Char(SIZE, font, L' ', color, true));
	glyph_list.insert(CarriagePosition, new Char(SIZE, font, L' ', color, true));
}
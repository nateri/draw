// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsx.h>
#include "draw.h"
#include "IDrawer.h"
#include "IEventHandler.h"
#include <vector>
#include "Canvas.h"
#include "Filler.h"
#include <iostream>

using namespace std;

typedef vector<IDrawer*> Drawers;
typedef vector<IEventHandler*> EventHandlers;

const size_t MAX_LOADSTRING = 100;
const size_t FRAME_SIZE = 16;
const size_t MENU_HEIGHT = 42;
const size_t CANVAS_WIDTH = 500;
const size_t CANVAS_HEIGHT = 500;

struct AppContext {
	// Handle of ... Instance
	HINSTANCE hInst;
	// Handle of Window
	HWND hWnd;
	// Window Class
	ATOM wClass;
	// Handle of Draw Context
	HDC hDc;
	// Title Bar text
	WCHAR title[MAX_LOADSTRING];
	// Main window class name
	WCHAR windowClass[MAX_LOADSTRING];
	// Objects that WM_PAINT
	Drawers drawers;
	// Objects that handle Events
	EventHandlers evtHandlers;

	Canvas* canvas;
	Filler* filler;
	COLORREF color;
};
AppContext gApp;


// Forward declarations of functions included in this code module:
BOOL				CreateApp(HINSTANCE, string);
VOID				DestroyApp();
BOOL				InitDrawers(HDC);
INT					RunMessageLoop(HINSTANCE);

LRESULT CALLBACK    cbWindowEvent(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);

	wstring cmdLineW = lpCmdLine;
	string cmdLine = string(cmdLineW.begin(), cmdLineW.end());
	if (!CreateApp(hInstance, cmdLine)) {
		return -1;
	}

	int result = RunMessageLoop(hInstance);
	return result;
}

int RunMessageLoop(HINSTANCE hInst)
{
    HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_DRAW));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
//   PURPOSE: Creates application context, registers window class,
//		      initializes main window
//
BOOL CreateApp(HINSTANCE hInstance, string cmdLine)
{
	srand(0x13);
	int f = rand();
	gApp.color = RGB(f, f, f);

	gApp.hInst = hInstance; // Store instance handle in our global variable

	// Initialize global strings
	WCHAR w[MAX_LOADSTRING], h[MAX_LOADSTRING];
	LoadStringW(hInstance, IDS_APP_TITLE, gApp.title, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DRAW, gApp.windowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDN_WIDTH, w, MAX_LOADSTRING);
	LoadStringW(hInstance, IDN_HEIGHT, h, MAX_LOADSTRING);


	// Register the Window Class
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = cbWindowEvent;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DRAW);
    wcex.lpszClassName  = gApp.windowClass;
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    gApp.wClass = RegisterClassExW(&wcex);
	if (!gApp.wClass) {
		return FALSE;
	}

	// Create Window
	int width = CANVAS_WIDTH + FRAME_SIZE;
	int height = CANVAS_HEIGHT + FRAME_SIZE + MENU_HEIGHT;
	gApp.hWnd = CreateWindowW(gApp.windowClass, gApp.title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		nullptr, nullptr, hInstance, nullptr);
	if (!gApp.hWnd) {
		return FALSE;
	}

	gApp.hDc = GetDC(gApp.hWnd);

	gApp.canvas = new Canvas(CANVAS_WIDTH, CANVAS_HEIGHT, RGB(0, 99, 199));
	gApp.canvas->Init(gApp.hWnd, gApp.hDc);
	gApp.canvas->Scratch();
	gApp.drawers.push_back(gApp.canvas);

	gApp.filler = new Filler(gApp.hWnd, gApp.canvas);
	gApp.filler->SetColor(gApp.color);
	gApp.evtHandlers.push_back(gApp.filler);

	ShowWindow(gApp.hWnd, TRUE);
	UpdateWindow(gApp.hWnd);

	return TRUE;
}

VOID DestroyApp()
{
	gApp.drawers.clear();
	gApp.evtHandlers.clear();
	delete gApp.filler;
	delete gApp.canvas;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK cbWindowEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case IDV_RAND_COLOR:
		{
			gApp.color = RGB(rand()%255, rand() % 255, rand() % 255);
			gApp.filler->SetColor(gApp.color);
		}
		break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(gApp.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			cout << "Resized: " << width << "x" << height << endl;
		}
		break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			for (const auto& d : gApp.drawers) {
				d->Draw(hdc);
			}
            EndPaint(hWnd, &ps);

			PostMessage(hWnd, IDV_RAND_COLOR, 0, 0);
        }
        break;

	case WM_LBUTTONDOWN:
		{
			for (const auto& h : gApp.evtHandlers) {
				EventContext c;
				c.mouseButton.id = MouseButtonId::Left;
				c.mouseButton.state = ButtonChange::Down;
				c.mouseButton.x = GET_X_LPARAM(lParam);
				c.mouseButton.y = GET_Y_LPARAM(lParam);
				c.mouseButton.flags = (DWORD)wParam;
				h->Handle(Event::MouseButton, c);
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			for (const auto& h : gApp.evtHandlers) {
				EventContext c;
				c.mouseButton.id = MouseButtonId::Left;
				c.mouseButton.state = ButtonChange::Up;
				c.mouseButton.x = GET_X_LPARAM(lParam);
				c.mouseButton.y = GET_Y_LPARAM(lParam);
				c.mouseButton.flags = (DWORD)wParam;
				h->Handle(Event::MouseButton, c);
			}
		}
		break;

    case WM_DESTROY:
		DestroyApp();
        PostQuitMessage(EXIT_SUCCESS);
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

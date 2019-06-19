#include <time.h>
#include <windows.h>
#include "Map.h"
#include "GenomeAI.h"
#include "Defines.h"

static HDC hdc;
static HDC hdcBackBuffer;
static HBITMAP hDefaultBitMap;
static HBITMAP hBitMap;

static int xClient, yClient;

static GenomeAI bob;

LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(static_cast <unsigned>(time(0)));

	static LPCTSTR windowClassName = "AI Techniques Tutorial";
	static LPCTSTR applicationName = "AI Tutorial";

	WNDCLASSEX winClass;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = windowClassName;
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winClass))
	{
		MessageBox(NULL, "Class Registration Failed!", "Error", 0);
		return 0;
	}

	HWND hWnd = CreateWindowEx(NULL, windowClassName, applicationName, WS_OVERLAPPEDWINDOW, 0, 0,
							   WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	bool done = false;
	MSG msg;

	while (!done)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				done = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// UPDATE LOOP

		bob.Run();

		// UPDATE END

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}

	UnregisterClass(windowClassName, hInstance);

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;

	const int NumLinesPerSide = 25;
	int xStep = xClient / NumLinesPerSide;
	int yStep = yClient / NumLinesPerSide;

	switch (msg)
	{
	case WM_CREATE:
		PlaySound("window_open.wav", NULL, SND_FILENAME | SND_ASYNC);
		GetClientRect(hWnd, &rect);
		xClient = rect.right;
		yClient = rect.bottom;

		hdc = GetDC(hWnd);
		hdcBackBuffer = CreateCompatibleDC(NULL);
		hBitMap = CreateCompatibleBitmap(hdc, xClient, yClient);
		hDefaultBitMap = (HBITMAP)SelectObject(hdcBackBuffer, hBitMap);

		bob = GenomeAI();

		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);

		BitBlt(hdcBackBuffer, 0, 0, xClient, yClient, NULL, NULL, NULL, WHITENESS);

		// RENDER LOOP

		bob.Render(xClient, yClient, hdcBackBuffer);

		// RENDER END

		BitBlt(ps.hdc, 0, 0, xClient, yClient, hdcBackBuffer, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		return 0;
	case WM_SIZE:
		hdc = GetDC(hWnd);

		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);

		SelectObject(hdcBackBuffer, hDefaultBitMap);
		DeleteObject(hBitMap);

		hBitMap = CreateCompatibleBitmap(hdc, xClient, yClient);

		SelectObject(hdcBackBuffer, hBitMap);

		ReleaseDC(hWnd, hdc);
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		case VK_LCONTROL:
			bob.Start();
		}
	case WM_DESTROY:
		DeleteObject(SelectObject(hdcBackBuffer, hDefaultBitMap));
		DeleteDC(hdcBackBuffer);

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

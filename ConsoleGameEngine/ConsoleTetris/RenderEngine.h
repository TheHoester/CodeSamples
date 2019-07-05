#pragma once
#include <Windows.h>

namespace Engine { namespace Graphics {
	/**
	 * RenderEngine
	 * Handles drawing to the console.
	 */
	class RenderEngine
	{
	private:
		HANDLE console;
		DWORD bytesWritten;
		SMALL_RECT windowRect;

		int screenWidth;
		int screenHeight;

	public:
		RenderEngine(int width, int height, int fontWidth = 8, int fontHeight = 16);
		~RenderEngine(void);

		void SetupWindow(const int& width, const int& height, const int& fontWidth = 8, const int& fontHeight = 16);
		void Draw(const wchar_t* title, const CHAR_INFO* characterArray);
	};
} }
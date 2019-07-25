#pragma once
#include <iostream>
#include <Windows.h>

#include "Singleton.h"
#include "Time.h"

namespace Engine { namespace Graphics {
	/**
	 * RenderEngine
	 * Handles drawing to the console.
	 */
	class RenderEngine : public Singleton<RenderEngine>
	{
		friend class Singleton<RenderEngine>;

	private:
		HANDLE console;
		DWORD bytesWritten;
		SMALL_RECT windowRect;

		int screenWidth;
		int screenHeight;

		RenderEngine(void) { }
		
	public:
		~RenderEngine(void);

		void SetupWindow(const int& width, const int& height, const int& fontWidth = 8, const int& fontHeight = 16);
		void Draw(const wchar_t* title, const CHAR_INFO* characterArray, const float& runTime, const float& renderTime);
	};
} }
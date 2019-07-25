#pragma once
#include <iostream>
#include "FVector2.h"

using namespace Engine::Physics;

namespace Engine { namespace Graphics {
	/**
	 * Sprite
	 * Holds the colour and pixel information for a 2D sprite.
	 */
	class Sprite
	{
	private:
		short* pixels;
		short* colours;

		int width;
		int height;
		FVector2 scale;

	public:
		Sprite(void);
		Sprite(int width, int height, float scaleX = 1.0f, float scaleY = 1.0f);
		Sprite(std::wstring filename);
		~Sprite(void);

		int Width() const;
		int Height() const;
		const FVector2& Scale() const;
		short GetPixel(const int& x, const int& y) const;
		short GetColour(const int& x, const int& y) const;
		short SamplePixel(const float& x, const float& y) const;
		short SampleColour(const float& x, const float& y) const;
		void SetScale(const float& x, const float& y);
		void SetPixel(const int& x, const int& y, const short& pixel);
		void SetColour(const int& x, const int& y, const short& colour);

		bool Save(std::wstring filename);
		bool Load(std::wstring filename);
	};
} }
#pragma once

#include "FVector2.h"
#include "Sprite.h"

using namespace Engine::Graphics;

namespace Engine 
{
	/**
	 * GameObject
	 * Contains all the basic elements that a standard GameObject would need.
	 * Should be overridden for custom behaviour.
	 */
	class GameObject
	{
	private:
		Sprite* sprite;

	public:
		FVector2 worldPosition;
		FVector2 screenPosition;
		bool isActive;

		GameObject(float x = 0.0f, float y = 0.0f, Sprite* sprite = nullptr);
		GameObject(FVector2 pos, Sprite* sprite = nullptr);
		~GameObject(void);

		Sprite* GetSprite(void) const;
		void SetSprite(Sprite* newSprite);
	};
}
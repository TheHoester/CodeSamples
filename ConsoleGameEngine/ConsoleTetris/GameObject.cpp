#include "GameObject.h"

/**
 * Constructor
 * @param x The X position of the game object in screenspace.
 * @param y The Y position of the game object in screenspace.
 * @param sprite The sprite to be displayed.
 */
Engine::GameObject::GameObject(float x, float y, Sprite* sprite) : worldPosition(x, y), screenPosition(x, y), sprite(sprite), isActive(true) { }

/**
 * Constructor
 * @param position The position of the game object in screenspace.
 * @param sprite The sprite to be displayed.
 */
Engine::GameObject::GameObject(FVector2 position, Sprite* sprite) : worldPosition(position), screenPosition(position), sprite(sprite), isActive(true) { }

/**
 * Destructor
 */
Engine::GameObject::~GameObject() { }

/**
 * GetSprite()
 * Will return a pointer to the sprite being used to display this GameObject.
 * @return Pointer to the sprite (could be NULL).
 */
Sprite* Engine::GameObject::GetSprite() const { return sprite; }

/**
 * SetSprite()
 * Will set a sprite pointer as the current sprite to be used to display this GameObject.
 * @param Pointer to the sprite (will accept NULL).
 */
void Engine::GameObject::SetSprite(Sprite* newSprite) { sprite = newSprite; }
#include "Time.h"

/**
 * Constructor
 */
Engine::Time::Time() : previousTime(std::chrono::system_clock::now()), currentTime(std::chrono::system_clock::now()), deltaTime(0.0f) { }

/**
 * Destructor
 */
Engine::Time::~Time() { }

/**
 * GetDeltaTime()
 * @return The length of time between the last frame and the current frame.
 */
float Engine::Time::GetDeltaTime() const { return deltaTime; }

/**
 * Update()
 * Handles the update of the time for the current frame.
 */
void Engine::Time::Update()
{
	currentTime = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = currentTime - previousTime;
	previousTime = currentTime;
	deltaTime = elapsedTime.count();
}
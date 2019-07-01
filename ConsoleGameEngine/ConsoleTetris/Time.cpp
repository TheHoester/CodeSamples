#include "Time.h"

/**
 * Constructor
 */
Time::Time() : previousTime(chrono::system_clock::now()), currentTime(chrono::system_clock::now()), deltaTime(0.0f) { }

/**
 * Destructor
 */
Time::~Time() { }

/**
 * GetDeltaTime()
 * @return The length of time between the last frame and the current frame.
 */
float Time::GetDeltaTime() const { return deltaTime; }

/**
 * Update()
 * Handles the update of the time for the current frame.
 */
void Time::Update()
{
	currentTime = chrono::system_clock::now();
	chrono::duration<float> elapsedTime = currentTime - previousTime;
	previousTime = currentTime;
	deltaTime = elapsedTime.count();
}
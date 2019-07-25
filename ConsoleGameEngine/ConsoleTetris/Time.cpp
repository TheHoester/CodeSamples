#include "Time.h"

/**
 * Constructor
 */
Engine::Time::Time() : previousTime(std::chrono::system_clock::now()), currentTime(std::chrono::system_clock::now()), deltaTime(0.0f), timeSinceStart(0.0f) { }

/**
 * Destructor
 */
Engine::Time::~Time() { }

/**
 * DeltaTime()
 * @return The length of time between the last frame and the current frame.
 */
float Engine::Time::DeltaTime() const { return deltaTime; }

/**
 * TimeSinceStart()
 * @return The time in seconds since the start of the game.
 */
float Engine::Time::TimeSinceStart() const { return timeSinceStart; }


/*
 * ConvertSecondsToTime()
 * Converts a time in seconds to a string that displays the minutes, seconds and milliseconds.
 * @param time The time in second.
 * @return The seconds converted to a string showing the minutes, seconds and milliseconds.
 */
std::wstring Engine::Time::ConvertSecondsToTime(const float& time)
{
	int minutes = time / 60.0f;
	int seconds = time - (minutes * 60.0f);
	int milliseconds = (time - seconds) * 1000.0f;
	return std::to_wstring(minutes) + L"." + std::to_wstring(seconds) + L":" + std::to_wstring(milliseconds);
}

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
	timeSinceStart += elapsedTime.count();
}

/**
 * Start()
 * Will initialize the time instance.
 * Calling this allows for an instance of the time class to be built without waiting for 
 * the first call for the instance.
 */
void Engine::Time::Start()
{
	Instance();
}
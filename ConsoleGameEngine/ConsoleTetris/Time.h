#pragma once
#include <chrono>

using namespace std;

/*
 * Time
 * Keeps track of the different time elements of the program.
 */
class Time
{
private:
	chrono::time_point<chrono::system_clock> previousTime;
	chrono::time_point<chrono::system_clock> currentTime;
	float deltaTime;

public:
	Time(void);
	~Time(void);

	float GetDeltaTime(void) const;

	void Update(void);
};
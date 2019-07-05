#pragma once
#include <chrono>

namespace Engine
{
	/*
	 * Time
	 * Keeps track of the different time elements of the program.
	 */
	class Time
	{
	private:
		std::chrono::time_point<std::chrono::system_clock> previousTime;
		std::chrono::time_point<std::chrono::system_clock> currentTime;
		float deltaTime;

	public:
		Time(void);
		~Time(void);

		float GetDeltaTime(void) const;

		void Update(void);
	};
}
#pragma once
#include <chrono>
#include <string>

#include "Singleton.h"

namespace Engine
{
	/*
	 * Time
	 * Keeps track of the different time elements of the program.
	 */
	class Time : public Singleton<Time>
	{
		friend class Singleton<Time>;
	private:
		std::chrono::time_point<std::chrono::system_clock> previousTime;
		std::chrono::time_point<std::chrono::system_clock> currentTime;
		float timeSinceStart;
		float deltaTime;

		Time(void);
	public:
		~Time(void);
		float DeltaTime(void) const;
		float TimeSinceStart(void) const;

		static std::wstring ConvertSecondsToTime(const float& time);

		static void Start(void);
		void Update(void);
	};
}
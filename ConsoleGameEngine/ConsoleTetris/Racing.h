#pragma once
#include <list>
#include <vector>

#include "Application.h"

class Racing : public Application
{
private:
	float carPosition;
	float distance;
	float speed;
	int direction;

	float curve;
	float trackCurve;
	float playerCurve;

	std::vector<std::pair<float, float>> track; // Curve, Distance
	float trackDistance;
	int currentSection;

	float currentLapTime;
	std::list<float> lapTimes;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;
public:
	Racing(GameEngine* engine, int appID, int width = 120, int height = 80, int fontWidth = 8, int fontHeight = 8);
	~Racing(void);

	int Update(void) override;
};
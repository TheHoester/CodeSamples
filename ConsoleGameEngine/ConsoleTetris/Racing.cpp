#include "Racing.h"


Racing::Racing(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	carPosition(0.0f), distance(0.0f), speed(0.0f), curve(0.0f), trackCurve(0.0f), playerCurve(0.0f), currentLapTime(0.0f) 
{
	GenerateAssets();
}

Racing::~Racing()  { }

int Racing::Update()
{
	GameLogic();
	Draw();

	if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
		Reset();
	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
	{
		Reset();
		return 0;
	}

	return appID;
}

void Racing::GameLogic()
{
	currentLapTime += Time::Instance().DeltaTime();

	// Player input
	if (InputHandler::Instance().IsKeyHeld(VK_UP))
		speed += 2.0f * Time::Instance().DeltaTime();
	else
		speed -= 1.0f * Time::Instance().DeltaTime();

	if (InputHandler::Instance().IsKeyHeld(VK_LEFT))
	{
		playerCurve -= 0.7f * Time::Instance().DeltaTime();
		direction = -1;
	}
	else if (InputHandler::Instance().IsKeyHeld(VK_RIGHT))
	{
		playerCurve += 0.7f * Time::Instance().DeltaTime();
		direction = 1;
	}
	else
		direction = 0;

	if (fabs(playerCurve - trackCurve) >= 0.8f)
		speed -= 5.0f * Time::Instance().DeltaTime();

	if (speed < 0.0f) speed = 0.0f;
	if (speed > 1.0f) speed = 1.0f;

	// Move car
	distance += (70.0f * speed) * Time::Instance().DeltaTime();

	// Get point on track
	float offset = 0.0f;
	currentSection = 0;

	if (distance >= trackDistance)
	{
		lapTimes.push_back(currentLapTime);
		if (lapTimes.size() > 5)
			lapTimes.pop_front();
		currentLapTime = 0.0f;
		distance -= trackDistance;
	}

	while (currentSection < track.size() && offset <= distance)
	{
		offset += track[currentSection].second;
		++currentSection;
	}

	// Update curve at current point
	curve += (track[currentSection - 1].first - curve) * speed * Time::Instance().DeltaTime();
	trackCurve += curve * speed * Time::Instance().DeltaTime();
}

void Racing::Draw() 
{
	// Draw Sky
	for (int y = 0 / 2; y < screenHeight / 2; ++y)
		for (int x = 0; x < screenWidth; ++x)
			engine->DrawChar(x, y, (y < screenHeight / 4) ? PIXEL_HALF : PIXEL_SOLID, FG_DARK_BLUE);

	// Draw Scenery
	for (int x = 0; x < screenWidth; ++x)
	{
		int hillHeight = (int)(fabs(sinf((x * 0.01f) + trackCurve) * 16.0f));
		for (int y = (screenHeight / 2) - hillHeight; y < screenHeight / 2; ++y)
			engine->DrawChar(x, y, PIXEL_SOLID, FG_DARK_YELLOW);
	}

	// Draw Road
	for (int y = 0 / 2; y < screenHeight / 2; ++y)
	{
		for (int x = 0; x < screenWidth; ++x)
		{
			float perspective = (float)y / (screenHeight / 2.0f);

			float middlePoint = 0.5f + (curve * powf(1.0f - perspective, 3));
			float roadWidth = 0.1f + (perspective * 0.8f);
			float clipWidth = roadWidth * 0.15f;

			roadWidth *= 0.5f;

			int leftGrass = (middlePoint - roadWidth - clipWidth) * screenWidth;
			int leftClip = (middlePoint - roadWidth) * screenWidth;
			int rightGrass = (middlePoint + roadWidth + clipWidth) * screenWidth;
			int rightClip = (middlePoint + roadWidth) * screenWidth;
			int row = (screenHeight / 2) + y;

			int grassColour = (sinf(20.0f * powf(1.0f - perspective, 3) + distance * 0.1f)) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
			int clipColour = (sinf(80.0f * powf(1.0f - perspective, 3) + distance * 0.1f)) > 0.0f ? FG_RED : FG_WHITE;

			int roadColour = ((currentSection - 1) == 0) ? FG_WHITE : FG_GREY;

			if ((x >= 0 && x < leftGrass) || (x >= rightGrass && x < screenWidth))
				engine->DrawChar(x, row, PIXEL_SOLID, grassColour);
			else if ((x >= leftGrass && x < leftClip) || (x >= rightClip && x < rightGrass))
				engine->DrawChar(x, row, PIXEL_SOLID, clipColour);
			else if (x >= leftClip && x < rightClip)
				engine->DrawChar(x, row, PIXEL_SOLID, roadColour);
		}
	}

	//Draw Car
	carPosition = playerCurve - trackCurve;
	int carScreenPos = screenWidth / 2 + ((int)(screenWidth * carPosition) / 2.0f) - 7;

	if (direction == 0)
	{
		engine->DrawStringAlpha(carScreenPos, 70, L"   ||####||   ");
		engine->DrawStringAlpha(carScreenPos, 71, L"      ##      ");
		engine->DrawStringAlpha(carScreenPos, 72, L"     ####     ");
		engine->DrawStringAlpha(carScreenPos, 73, L"     ####     ");
		engine->DrawStringAlpha(carScreenPos, 74, L"|||  ####  |||");
		engine->DrawStringAlpha(carScreenPos, 75, L"|||########|||");
		engine->DrawStringAlpha(carScreenPos, 76, L"|||  ####  |||");
	}
	else if (direction == 1)
	{
		engine->DrawStringAlpha(carScreenPos, 70, L"      //####//");
		engine->DrawStringAlpha(carScreenPos, 71, L"         ##   ");
		engine->DrawStringAlpha(carScreenPos, 72, L"       ####   ");
		engine->DrawStringAlpha(carScreenPos, 73, L"      ####    ");
		engine->DrawStringAlpha(carScreenPos, 74, L"///  ####//// ");
		engine->DrawStringAlpha(carScreenPos, 75, L"//#######///O ");
		engine->DrawStringAlpha(carScreenPos, 76, L"/// #### //// ");
	}
	else if (direction == -1)
	{
		engine->DrawStringAlpha(carScreenPos, 70, L"\\\\####\\\\      ");
		engine->DrawStringAlpha(carScreenPos, 71, L"   ##         ");
		engine->DrawStringAlpha(carScreenPos, 72, L"   ####       ");
		engine->DrawStringAlpha(carScreenPos, 73, L"    ####      ");
		engine->DrawStringAlpha(carScreenPos, 74, L" \\\\\\\\####  \\\\\\");
		engine->DrawStringAlpha(carScreenPos, 75, L" O\\\\########\\\\");
		engine->DrawStringAlpha(carScreenPos, 76, L" \\\\\\\\ #### \\\\\\");
	}


	engine->DrawString(0, 0, L"Distance: " + std::to_wstring(distance));
	engine->DrawString(0, 1, L"Target Curve: " + std::to_wstring(curve));
	engine->DrawString(0, 2, L"Player Curve: " + std::to_wstring(playerCurve));
	engine->DrawString(0, 3, L"Player Speed: " + std::to_wstring(speed));
	engine->DrawString(0, 4, L"Track Curve: " + std::to_wstring(trackCurve));
	engine->DrawString(0, 6, L"Lap Time: " + Time::Instance().ConvertSecondsToTime(currentLapTime));

	int i = 0;
	for (float lapTime : lapTimes)
	{
		engine->DrawString(0, 8 + i, L"Time: " + Time::Instance().ConvertSecondsToTime(lapTime));
		++i;
	}
}

void Racing::Reset()
{
	carPosition = 0.0f;
	distance = 0.0f;
	speed = 0.0f;
	direction = 0;

	curve = 0.0f;
	trackCurve = 0.0f;
	playerCurve = 0.0f;
	currentSection = 0;

	currentLapTime = 0.0f;

	while (!lapTimes.empty())
		lapTimes.pop_back();
}

void Racing::GenerateAssets()
{
	track.push_back(std::make_pair(0.0f, 10.0f));
	track.push_back(std::make_pair(0.0f, 200.0f));
	track.push_back(std::make_pair(1.0f, 200.0f));
	track.push_back(std::make_pair(0.0f, 400.0f));
	track.push_back(std::make_pair(-1.0f, 100.0f));
	track.push_back(std::make_pair(0.0f, 200.0f));
	track.push_back(std::make_pair(-1.0f, 200.0f));
	track.push_back(std::make_pair(1.0f, 200.0f));
	track.push_back(std::make_pair(0.0f, 200.0f));
	track.push_back(std::make_pair(0.2f, 500.0f));
	track.push_back(std::make_pair(0.0f, 200.0f));

	trackDistance = 0.0f;
	for (auto piece : track)
		trackDistance += piece.second;
}
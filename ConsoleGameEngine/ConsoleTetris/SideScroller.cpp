#include "SideScroller.h"

/*
 * Constructor
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
SideScroller::SideScroller(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	backgroundXPos(0.0f), playerSpeed(40.f), playerVelocity(0.0f, 0.0f), jumpForce(100.0f), isJumping(false)
{
	GenerateAssets();
}

/**
 * Destructor
 */
SideScroller::~SideScroller()
{
	if (background1 != nullptr)
		delete background1;
	if (ground1 != nullptr)
		delete ground1;
	if (player1 != nullptr)
		delete player1;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int SideScroller::Update()
{
	if (!playerObject->isActive)
		playerObject->isActive = true;

	GameLogic();
	Draw();

	if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
		Reset();
	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
	{
		playerObject->isActive = false;
		Reset();
		return 0;
	}

	return appID;
}

/*
 * GameLogic()
 * Runs the main logic for the game.
 */
void SideScroller::GameLogic()
{
	// INPUT ##################################################################################################################################################
	
	// Movement
	if (InputHandler::Instance().IsKeyHeld('D')) // Move Left
	{
		playerObject->GetSprite()->SetScale(1.0f, 1.0f);
		playerVelocity.x = playerSpeed;
	}
	else if (InputHandler::Instance().IsKeyHeld('A')) // Move Right
	{
		playerObject->GetSprite()->SetScale(-1.0f, 1.0f);
		playerVelocity.x = -playerSpeed;
	}
	else
		playerVelocity.x = 0.0f;

	// Jump
	if (!isJumping && InputHandler::Instance().IsKeyPressed('W'))
	{
		isJumping = true;
		playerVelocity.y -= jumpForce;
	}

	// LOGIC ##################################################################################################################################################
	//backgroundXPos -= 10.0f * Time::Instance().DeltaTime();
	//if (backgroundXPos < -background1->Width())
		//backgroundXPos = 0;

	// Gravity
	if (isJumping)
		playerVelocity.y += GRAVITY;
	
	// Forces on player
	playerObject->screenPosition += playerVelocity * Time::Instance().DeltaTime();

	if (playerObject->screenPosition.x < 0)
		playerObject->screenPosition.x = 0;
	else if (playerObject->screenPosition.x + playerObject->GetSprite()->Width() >= screenWidth)
		playerObject->screenPosition.x = screenWidth - playerObject->GetSprite()->Width();

	if (isJumping && playerObject->screenPosition.y > screenHeight - ground1->Height() - player1->Height())
	{
		isJumping = false;
		playerVelocity.y = 0.0f;
		playerObject->screenPosition.y = screenHeight - ground1->Height() - player1->Height();
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void SideScroller::Draw()
{
	// Draw Background
	engine->DrawSprite(backgroundXPos, 0, *background1);
	engine->DrawSprite(backgroundXPos + background1->Width(), 0, *background1);

	// Draw Ground
	for (int i = 0; i < (screenWidth / ground1->Width()); ++i)
		engine->DrawSprite(i * ground1->Width(), screenHeight - ground1->Height(), *ground1);
}

/**
 * ResetGame()
 * Resets the game back to it's starting state.
 */
void SideScroller::Reset()
{
}

void SideScroller::GenerateAssets() 
{ 
	background1 = new Sprite(L"../Assets/SideScroller/Background1.spr");
	background1->SetScale(4.0f, 4.0f);
	ground1 = new Sprite(L"../Assets/SideScroller/Ground1.spr");
	player1 = new Sprite(L"../Assets/SideScroller/Player1.spr");

	playerObject = engine->CreateGameObject(5, screenHeight - (ground1->Height() * 2), player1);
	playerObject->isActive = false;
}
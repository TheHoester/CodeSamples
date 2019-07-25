#include "ThreeDimentions.h"

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
 * @param radus The radius of the ball.
 */
ThreeDimentions::ThreeDimentions(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	fov(90.0f), farClippingPlane(1000.0f), nearClippingPlane(0.1f), aspectRatio(height / width), xRotSpeed(1.0f), yRotSpeed(0.0f), zRotSpeed(0.75f), xRotAngle(0.0f), yRotAngle(0.0f), zRotAngle(0.0f),
	cameraPos(), cameraUp(0.0f, 1.0f, 0.0f), lookDirection(0.0f, 0.0f, 1.0f), cameraTarget(cameraPos + lookDirection), yaw(0.0f)
{ 
	GenerateAssets();
}

/**
 * Destructor
 */
ThreeDimentions::~ThreeDimentions()
{
	delete cubeMesh;
	delete axisMesh;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int ThreeDimentions::Update()
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

/*
 * GameLogic()
 * Runs the main logic for the game.
 */
void ThreeDimentions::GameLogic()
{
	// Rotation update
	/*xRotAngle += xRotSpeed * Time::Instance().DeltaTime();
	yRotAngle += yRotSpeed * Time::Instance().DeltaTime();
	zRotAngle += zRotSpeed * Time::Instance().DeltaTime();*/

	// Elevation
	if (InputHandler::Instance().IsKeyHeld(VK_UP))
		cameraPos.y += 8.0f * Time::Instance().DeltaTime();
	if (InputHandler::Instance().IsKeyHeld(VK_DOWN))
		cameraPos.y -= 8.0f * Time::Instance().DeltaTime();


	// Forward
	FVector3 forward = lookDirection * (8.0f * Time::Instance().DeltaTime());
	if (InputHandler::Instance().IsKeyHeld('W'))
		cameraPos += forward;
	if (InputHandler::Instance().IsKeyHeld('S'))
		cameraPos -= forward;

	// Strafe
	FVector3 right = (lookDirection.CrossProduct(cameraUp).Normalized()) * (8.0f * Time::Instance().DeltaTime());
	if (InputHandler::Instance().IsKeyHeld('E'))
		cameraPos += right;
	if (InputHandler::Instance().IsKeyHeld('Q'))
		cameraPos -= right;

	// Rotation
	if (InputHandler::Instance().IsKeyHeld('A'))
		yaw -= 2.0f * Time::Instance().DeltaTime();
	if (InputHandler::Instance().IsKeyHeld('D'))
		yaw += 2.0f * Time::Instance().DeltaTime();

	cameraTarget = FVector3(0.0f, 0.0f, 1.0f);
	Matrix4x4 cameraRotMat = Matrix4x4::RotationYMatrix(yaw);
	lookDirection = cameraTarget * cameraRotMat;
	cameraTarget = cameraPos + lookDirection;

	cameraMat = Matrix4x4::PointAtMatrix(cameraPos, cameraTarget, cameraUp);

	rotXMat = Matrix4x4::RotationXMatrix(xRotAngle);
	rotYMat = Matrix4x4::RotationYMatrix(yRotAngle);
	rotZMat = Matrix4x4::RotationZMatrix(zRotAngle);
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void ThreeDimentions::Draw()
{
	engine->ClearScreen();

	Matrix4x4 viewMat = Matrix4x4::InvertPointAtMatrix(cameraMat);

	// Object Matrises
	Matrix4x4 transformMat = Matrix4x4::TranslationMatrix(0.0f, 0.0f, 6.0f);
	Matrix4x4 worldMat = rotZMat * rotXMat;
	worldMat *= rotYMat;
	worldMat *= transformMat;

	// Stores triangle for rastering.
	std::vector<Triangle> triangleBuffer;

	for (auto tri : cubeMesh->tris)
	{
		Triangle triTransformed = tri * worldMat;

		// CROSS PRODUCT
		FVector3 normal = (triTransformed.points[1] - triTransformed.points[0]).CrossProduct(triTransformed.points[2] - triTransformed.points[0]);
		normal = normal.Normalized();

		// DOT PRODUCT - Culls tris on the back of the mesh (from camera perspective
		if (normal.DotProduct(triTransformed.points[0] - cameraPos) < 0.0f)
		{
			// Illumination
			FVector3 directionalLight = FVector3(0.0f, 1.0f, -1.0f);
			directionalLight = directionalLight.Normalized();

			CHAR_INFO c = GetColour(FG_RED, normal.DotProduct(directionalLight));
			triTransformed.pixel = c.Char.UnicodeChar;
			triTransformed.colour = c.Attributes;

			// Convert World Space -> View Space
			Triangle triViewed = triTransformed * viewMat;

			// Clipped viewed triangle against near plane
			int clippedTris = 0;
			Triangle clipped[2];					// Position of the plane in screen scace  // Normal in screen space  <- NOT WORLD SPACE 
			clippedTris = triViewed.ClipAgainstPlane(FVector3(0.0f, 0.0f, nearClippingPlane), FVector3(0.0f, 0.0f, 1.0f), clipped[0], clipped[1]);

			// Apply projection
			for (int i = 0; i < clippedTris; ++i)
			{
				Triangle triProjected = clipped[i] * projectionMat;
				triProjected.points[0].x *= -1.0f;
				triProjected.points[1].x *= -1.0f;
				triProjected.points[2].x *= -1.0f;
				triProjected.points[0].y *= -1.0f;
				triProjected.points[1].y *= -1.0f;
				triProjected.points[2].y *= -1.0f;

				triProjected += FVector3(1.0f, 1.0f, 0.0f);
				triProjected *= FVector3(0.5f * (float)screenWidth, 0.5f * (float)screenHeight, 1.0f);

				// Store triangle for sorting
				triangleBuffer.push_back(triProjected);
			}
		}
	}

	// Sort Triangle back to front
	sort(triangleBuffer.begin(), triangleBuffer.end(), [](Triangle &t1, Triangle &t2)
	{
		float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
		float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
		return z1 > z2;
	});

	for (auto& triToRaster : triangleBuffer)
	{
		// Clipping to the screen edges
		Triangle clipped[2];
		std::list<Triangle> triangleList;
		triangleList.push_back(triToRaster);
		int newTriangles = 1;

		FVector3 topLeft = FVector3(0.0f, 0.0f, 0.0f);
		FVector3 bottom = FVector3(0.0f, (float)screenHeight - 1.0f, 0.0f);
		FVector3 right = FVector3((float)screenWidth - 1.0f, 0.0f, 0.0f);

		for (int p = 0; p < 4; ++p)
		{
			int trisToAdd = 0;
			while (newTriangles > 0)
			{
				Triangle test = triangleList.front();
				triangleList.pop_front();
				--newTriangles;
				switch (p)
				{
				case 0: trisToAdd = test.ClipAgainstPlane(FVector3(0.0f, 0.0f, 0.0f),                       FVector3( 0.0f,  1.0f, 0.0f), clipped[0], clipped[1]); break;
				case 1: trisToAdd = test.ClipAgainstPlane(FVector3(0.0f, (float)screenHeight - 1.0f, 0.0f), FVector3( 0.0f, -1.0f, 0.0f), clipped[0], clipped[1]); break;
				case 2: trisToAdd = test.ClipAgainstPlane(FVector3(0.0f, 0.0f, 0.0f),                       FVector3( 1.0f,  0.0f, 0.0f), clipped[0], clipped[1]); break;
				case 3: trisToAdd = test.ClipAgainstPlane(FVector3((float)screenWidth - 1.0f, 0.0f, 0.0f),  FVector3(-1.0f,  0.0f, 0.0f), clipped[0], clipped[1]); break;
				}

				for (int w = 0; w < trisToAdd; ++w)
					triangleList.push_back(clipped[w]);
			}
			newTriangles = triangleList.size();
		}


		// Draw Tris
		for (auto &tri : triangleList)
		{
			engine->DrawFillTriangle(tri.points[0].x, tri.points[0].y, tri.points[1].x, tri.points[1].y, tri.points[2].x, tri.points[2].y, tri.pixel, tri.colour);
			//engine->DrawTriangle(tri.points[0].x, tri.points[0].y, tri.points[1].x, tri.points[1].y, tri.points[2].x, tri.points[2].y, PIXEL_SOLID, FG_BLACK);
		}
	}

	for (int i = 0; i < 19; ++i)
	{
		CHAR_INFO colour = GetColour(FG_MAGENTA, (float)(i) / 19.0f);
		engine->DrawRectFill(i * 9, 0, (i * 9) + 8, 24, colour.Char.UnicodeChar, colour.Attributes, colour.Char.UnicodeChar, colour.Attributes);
	}
}

/**
 * ResetGame()
 * Resets the game back to it's starting state.
 */
void ThreeDimentions::Reset()
{

}

void ThreeDimentions::GenerateAssets() 
{
	cubeMesh = new Mesh();
	cubeMesh->LoadFromObjFile("../Assets/Models/Head.obj");
	axisMesh = new Mesh();
	axisMesh->LoadFromObjFile("../Assets/Models/axis.obj");

	// Projection Matrix
	projectionMat = Matrix4x4::ProjectionMatrix(aspectRatio, fov, nearClippingPlane, farClippingPlane);
}

CHAR_INFO ThreeDimentions::GetColour(const float& lum)
{
	short bgCol, fgCol;
	short sym;
	int pixelBw = (int)(13.0f * lum);

	switch (pixelBw)
	{
	case 0: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_QUARTER; break;

	case 1: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_THREEQUARTER; break;
	case 4: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_THREEQUARTER; break;
	case 8: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_THREEQUARTER; break;
	case 12: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_SOLID; break;
	default: bgCol = BG_BLACK; fgCol = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO character;
	character.Char.UnicodeChar = sym;
	character.Attributes = bgCol | fgCol;
	return character;
}

CHAR_INFO ThreeDimentions::GetColour(const short& baseColour, const float& lum)
{
	short darkColour = baseColour - 8;

	short bgCol, fgCol;
	short sym;
	int pixelBw = (int)(19.0f * lum);

	switch (pixelBw)
	{
	case 0: bgCol = BG_BLACK; fgCol = BG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_QUARTER; break;
	case 2: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_HALF; break;

	case 3: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_QUARTER; break;
	case 4: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_HALF; break;

	case 5: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_THREEQUARTER; break;
	case 6: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 7: bgCol = BG_DARK_GREY; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 8: bgCol = BG_GREY; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 9: bgCol = BG_WHITE; fgCol = darkColour; sym = PIXEL_SOLID; break;

	case 10: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_THREEQUARTER; break;
	case 11: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 12: bgCol = BG_DARK_GREY; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 13: bgCol = BG_GREY; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 14: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 15: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_THREEQUARTER; break;
	case 16: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_HALF; break;
	case 17: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_QUARTER; break;

	case 18: bgCol = BG_WHITE; fgCol = FG_WHITE; sym = PIXEL_QUARTER; break;

	default: bgCol = BG_BLACK; fgCol = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO character;
	character.Char.UnicodeChar = sym;
	character.Attributes = bgCol | fgCol;
	return character;
}
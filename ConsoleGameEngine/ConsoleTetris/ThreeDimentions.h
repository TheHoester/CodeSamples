#pragma once
#include <algorithm>
#include <list>

#include "Application.h"
#include "Defines.h"
#include "FVector2.h"
#include "Matrix4x4.h"
#include "Mesh.h"

using namespace Engine::Graphics;
using namespace Engine::Physics;

class ThreeDimentions : public Application
{
private:
	Mesh* cubeMesh;
	Mesh* axisMesh;
	Matrix4x4 projectionMat;
	Matrix4x4 rotXMat;
	Matrix4x4 rotYMat;
	Matrix4x4 rotZMat;
	Matrix4x4 cameraMat;

	float fov;
	float farClippingPlane;
	float nearClippingPlane;
	float aspectRatio;
	float xRotSpeed;
	float yRotSpeed;
	float zRotSpeed;
	float xRotAngle;
	float yRotAngle;
	float zRotAngle;

	FVector3 cameraPos;
	FVector3 cameraUp;
	FVector3 lookDirection;
	FVector3 cameraTarget;
	float yaw;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;
	CHAR_INFO GetColour(const float& lum);
	CHAR_INFO GetColour(const short& baseColour, const float& lum);

public:
	ThreeDimentions(GameEngine* engine, int appID, int width = 160, int height = 160, int fontWidth = 4, int fontHeight = 4);
	~ThreeDimentions(void);

	int Update(void) override;
};
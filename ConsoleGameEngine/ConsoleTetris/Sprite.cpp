#include "Sprite.h"

using namespace Engine::Graphics;

/*
 * Constructor - Default.
 */
Sprite::Sprite() : Sprite(8, 8) {  }

/*
 * Constructor - Blank of set size.
 * @param width Pixel width of the sprite.
 * @param height Pixel height of the sprite.
 */
Sprite::Sprite(int width, int height, float scaleX, float scaleY) : width(width), height(height), scale(scaleX, scaleY)
{ 
	pixels = new short[width * height];
	colours = new short[width * height];
	memset(pixels, 0, sizeof(short) * width * height);
	memset(colours, 0, sizeof(short) * width * height);
}

/*
 * Constructor - Load from file.
 * @param filename The filename of the sprite to be loaded.
 */
Sprite::Sprite(std::wstring filename)
{
	if (!Load(filename))
		Sprite();
}

/*
 * Destructor
 */
Sprite::~Sprite()
{
	if (pixels != nullptr)
		delete[] pixels;
	if (colours != nullptr)
		delete[] colours;
}

int Sprite::Width() const { return width * abs(scale.x); }
int Sprite::Height() const { return height * abs(scale.y); }
const FVector2& Sprite::Scale() const { return scale; }

short Sprite::GetPixel(const int& x, const int& y) const
{
	if (scale.x == 0 || scale.y == 0)
		return 0;

	int pixelX = (int)((float)x / scale.x);
	int pixelY = (int)((float)y / scale.y);

	if (scale.x < 0)
		pixelX += width - 1;
	if (scale.y < 0)
		pixelY += height - 1;

	if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height)
		return pixels[(pixelY * width) + pixelX];
	else
		return 0;
}

short Sprite::GetColour(const int& x, const int& y) const
{
	if (scale.x == 0 || scale.y == 0)
		return 0;

	int colourX = (int)((float)x / scale.x);
	int colourY = (int)((float)y / scale.y);

	if (scale.x < 0)
		colourX += width - 1;
	if (scale.y < 0)
		colourY += height - 1;

	if (colourX >= 0 && colourX < width && colourY >= 0 && colourY < height)
		return colours[(colourY * width) + colourX];
	else
		return 0;
}

short Sprite::SamplePixel(const float& x, const float& y) const
{
	int sampleX = (int)(x * (float)width);
	int sampleY = (int)((y * (float)height) - 1.0f);

	if (sampleX >= 0 && sampleX < width && sampleY >= 0 && sampleY < height)
		return pixels[(sampleY * width) + sampleX];
	else
		return 0;
}

short Sprite::SampleColour(const float& x, const float& y) const
{
	int sampleX = (int)(x * (float)width);
	int sampleY = (int)((y * (float)height) - 1.0f);

	if (sampleX >= 0 && sampleX < width && sampleY >= 0 && sampleY < height)
		return colours[(sampleY * width) + sampleX];
	else
		return 0;
}

void Sprite::SetScale(const float& x, const float& y)
{
	scale.x = x;
	scale.y = y;
}

void Sprite::SetPixel(const int& x, const int& y, const short& pixel)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
		pixels[(y * width) + x] = pixel;
}

void Sprite::SetColour(const int& x, const int& y, const short& colour)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
		colours[(y * width) + x] = colour;
}

bool Sprite::Save(std::wstring filename)
{
	FILE* file = nullptr;
	_wfopen_s(&file, filename.c_str(), L"wb");
	if (file == nullptr)
		return false;

	fwrite(&width, sizeof(int), 1, file);
	fwrite(&height, sizeof(int), 1, file);
	fwrite(pixels, sizeof(short), width * height, file);
	fwrite(colours, sizeof(short), width * height, file);

	fclose(file);
	return true;
}

bool Sprite::Load(std::wstring filename)
{
	if (pixels != nullptr)
		delete[] pixels;
	if (colours != nullptr)
		delete[] colours;

	width = 0;
	height = 0;
	scale = FVector2(1.0f, 1.0f);

	FILE* file = nullptr;
	_wfopen_s(&file, filename.c_str(), L"rb");
	if (file == nullptr)
		return false;

	fread(&width, sizeof(int), 1, file);
	fread(&height, sizeof(int), 1, file);

	pixels = new short[width * height];
	colours = new short[width * height];
	memset(pixels, 0, sizeof(short) * width * height);
	memset(colours, 0, sizeof(short) * width * height);

	fread(pixels, sizeof(short), width * height, file);
	fread(colours, sizeof(short), width * height, file);

	fclose(file);
	return true;
}
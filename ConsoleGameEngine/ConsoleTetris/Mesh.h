#pragma once
#include <fstream>
#include <strstream>
#include <vector>

#include "FVector3.h"
#include "Triangle.h"

namespace Engine { namespace Graphics {
	class Mesh
	{
	public:
		std::vector<Triangle> tris;

		bool LoadFromObjFile(std::string filename);
	};
}}
#include "Mesh.h"

bool Engine::Graphics::Mesh::LoadFromObjFile(std::string filename)
{
	std::ifstream f(filename);
	if (!f.is_open())
		return false;

	std::vector<Physics::FVector3> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream ss;
		ss << line;

		char junk;

		if (line[0] == 'v')
		{
			Physics::FVector3 v;
			ss >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}
		else if (line[0] == 'f')
		{
			int f[3];
			ss >> junk >> f[0] >> f[1] >> f[2];
			tris.push_back(Triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
		}
	}
	f.close();
	return true;
}
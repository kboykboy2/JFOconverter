#pragma once
#include "Uasset.h"



namespace MeshTool
{
	// find vertex data and get offset of drop normal location
	Uint64 FindVertexOffset(Uint64 UexpSize);
	// go to offset and insert drop normals
	void InsertDropNormals(Uint64 offset, Uint64 uexpsize, std::string newpath);
	// fix uexp
	void FixMesh(string path, Uasset uasset);
}

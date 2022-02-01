/*
Original Code By: AlexPo
Modified Code By: Kboykboy
Mesh Research By: Vergilfox, Skreborn, AlexPo
*/
#include "MeshTool.h"
#include "BinaryUtils.h"
using namespace BinaryUtils;



// find vertex data and get offset of drop normal location
Uint64 MeshTool::FindVertexOffset( Uint64 UexpSize)
{
	using std::string;
	Uint64 endOffset = 0;

	unsigned int blocksize{};
	unsigned int blocksize2{};
	unsigned int blockcount{};
	// read entire file into buffer
	char* uexp = read();
	// loop each byte
	for (int i = 0; i < UexpSize; i++)
	{
		// check if this byte equals 12 and the byte 8 bytes ahead also equal 12
		if (uexp[i] == 12 and uexp[i+8] == 12)
		{
			// convert individual bytes to arrays of 4
			char int1[4]{uexp[i], uexp[i+1], uexp[i+2], uexp[i+3]};
			char intA[4]{uexp[i+4], uexp[i+5], uexp[i+6], uexp[i+7]};
			char int2[4]{uexp[i+8], uexp[i+9], uexp[i+10], uexp[i+11]};
			char intB[4]{uexp[i+12], uexp[i+13], uexp[i+14], uexp[i+15]};
			// convert byte arrays to integers
			memcpy(&blocksize, int1, sizeof(blocksize));
			memcpy(&blockcount, intA, sizeof(blockcount));
			// calculate vertex data length
			Uint64 datalength = (Uint64)blocksize * blockcount;
			// check if the fist int equals the third and the second equals the 4th and that the data length isn't greater than the file size
			if (string(int1) == string(int2) and string(intA) == string(intB) and blockcount > 3 and datalength+i < UexpSize)
			{
				// get int at end of vertex data to check if it equals the number of vertices
				Uint64 offset = (Uint64)i + 16 + datalength + 6;
				char bdata[4]{ uexp[offset],uexp[offset + 1],uexp[offset + 2],uexp[offset + 3] };
				if (string(bdata) == string(intA))
				{
					endOffset = offset - 6;
					break;
				}
			}

		}
	}
	delete[] uexp;
	return endOffset;
}


// go to offset and insert drop normals
void MeshTool::InsertDropNormals(Uint64 offset, Uint64 uexpsize, std::string newpath)
{
	// read old uexp
	GoTo(0);
	char* data1 = read(offset + 18);
	char* data2 = read();
	char dropNormals[4]{ 0,0,0,0 };
	// write new uexp
	std::fstream uexp(newpath, std::ios::out | std::ios::binary);
	uexp.write(data1, offset + 18);
	uexp.write(dropNormals, 4);
	uexp.write(data2, uexpsize - offset-18);
	uexp.close();
	// free memory
	delete[] data1;
	delete[] data2;
}


// fix uexp
void MeshTool::FixMesh(string path, Uasset uasset)
{
	OpenFile(path);
	Uint64 uexpSize = GetSize();
	Uint64 offset = FindVertexOffset(uexpSize);
	InsertDropNormals(offset, uexpSize, path + ".New");
	CloseFile();

	uasset.Fix(offset, 4);
}
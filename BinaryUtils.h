# pragma once
# include <windows.h> 
# include <iostream>
# include <fstream>
# include <vector>
# include <stdio.h> 
# include <bitset>
# include <cmath>
# include <list>
# include <algorithm>


namespace BinaryUtils
{
	///////////////////////////////
	// general utility functions //
	///////////////////////////////

	// check if a file is open
	bool isFileOpen();

	// decimal to binary
	std::string decToBinary(int i, int size = 16);

	// swap endian
	char* SwapEndian(char* bytes, int length);

	////////////////////////////
	// file utility functions //
	////////////////////////////

	// open file
	void OpenFile(std::string path, std::string fileEndian = "little");

	// close file
	void CloseFile();

	// go to specified location in file
	void GoTo(int pointer);

	// skip specified amount in file
	void skip(int amount);

	// skip specified amount in file
	void pad(int amount);

	// get location in file
	int GetLocation();

	// get file size
	int GetSize();

	//////////////////////////
	// read value functions //
	//////////////////////////

	// define read function (read file)
	char* read(int length = 0);

	// define int8 reading function (char)
	uint8_t int8();

	// define int16 reading function (short)
	uint16_t int16();

	// define int32 reading function (long)
	uint32_t int32();

	// define int64 reading function (long long)
	uint64_t int64();

	// define string reading function (string)
	std::string String(int length = -1);

	// define float16 reading function (half precision float) EEE 754-2008 (custom implementation)
	float float16(std::string endianOverride = "none");

	// define float32 reading function (full precision float)
	float float32(std::string endianOverride = "none");

	// define float64 reading function (double precision float)
	double float64(std::string endianOverride = "none");

	///////////////////////////////////
	// structs classes and type defs //
	///////////////////////////////////

	// set longlong
	typedef int64_t longlong;

	// set longlong
	typedef uint64_t unsigned_longlong;
}
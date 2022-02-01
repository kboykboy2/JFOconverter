# include "BinaryUtils.h"
using namespace std;
// globals
ifstream file;
string endian{};
bool FileOpen = false;

namespace BinaryUtils
{
	///////////////////////////////
	// general utility functions //---------------------------------------------
	///////////////////////////////

	bool isFileOpen()
	{
		return FileOpen;
	}

	// function to convert decimal to binary 
	string decToBinary(int i, int size)
	{
		string bin;
		while (i != 0) { bin = (i % 2 == 0 ? "0" : "1") + bin; i /= 2; }
		while (bin.length() < size)
		{
			bin.insert(0, "0");
		}
		return bin;
	}

	// swap endian
	char* SwapEndian(char* bytes, int length)
	{
		char* newbytes = new char[length];

		int i = length - 1;
		int n = 0;
		while (i >= 0)
		{
			newbytes[n] = bytes[i];
			i -= 1;
			n += 1;
		}
		return newbytes;
	}

	////////////////////////////
	// file utility functions //---------------------------------------------
	////////////////////////////

	// open file
	void OpenFile(string path, string fileEndian)
	{
		file.open(path, ios::in | ios::binary); // open file
		endian = fileEndian;
		FileOpen = true;
	}

	// set endian
	void SetEndian(string fileEndian)
	{
		endian = fileEndian;
	}

	// close file
	void CloseFile()
	{
		if (file.is_open())
		{
			file.close();
		}
		FileOpen = false;
	}


	// go to specified location in file
	void GoTo(int pointer)
	{
		file.seekg(pointer);
	}


	// skip specified amount in file
	void skip(int amount)
	{
		int loc = file.tellg();
		GoTo(loc + amount);
	}


	// skip specified amount in file
	void pad(int amount)
	{
		int loc = file.tellg();
		GoTo(loc + amount);
	}


	// get location in file
	int GetLocation()
	{
		int loc = file.tellg();
		return loc;
	}


	// get file size
	int GetSize()
	{
		int loc = file.tellg();
		file.seekg(0, ios::_Seekend);
		int size = file.tellg();
		GoTo(loc);
		return size;
	}

	//////////////////////////
	// read value functions //---------------------------------------------
	//////////////////////////

	// define read function (read file)
	char* read(int length)
	{
		// if argument empty read whole file
		if (length == 0)
		{
			int start = GetLocation();
			int size = GetSize();
			length = size - start;
			GoTo(start);
		}
		// read data
		char* data{};
		if (file.is_open())
		{
			data = new char[length];
			file.read(data, length);
		}
		return data;
	}

	// define int8 reading function (char)
	uint8_t int8()
	{
		uint8_t a{};
		char* bytes = new char[sizeof(a)];
		file.read((char*)&a, sizeof(a));
		return a;
	}


	// define int16 reading function (short)
	uint16_t int16()
	{
		uint16_t a{};
		char* bytes = new char[sizeof(a)];
		file.read((char*)&a, sizeof(a));
		return a;
	}


	// define int32 reading function (long)
	uint32_t int32()
	{
		uint32_t a{};
		char* bytes = new char[sizeof(a)];
		file.read((char*)&a, sizeof(a));
		return a;
	}


	// define int64 reading function (long long)
	uint64_t int64()
	{
		uint64_t a{};
		char* bytes = new char[sizeof(a)];
		file.read((char*)&a, sizeof(a));
		return a;
	}


	// define string reading function (string)
	string String(int length)
	{
		// if argument empty assume null terminated and find length by locating null
		if (length == -1)
		{
			int start = GetLocation();
			while (true)
			{
				length += 1;
				int byte = int8();
				if (byte == 0)
				{
					break;
				}
			}
			GoTo(start);
		}
		// read string
		string text;
		if (file.is_open())
		{
			text.resize(length);
			file.read((char*)&text[0], length);
		}
		// skip null byte if string is null terminated
		if (length == -1)
		{
			skip(1);
		}
		return text;
	}


	// define float16 reading function (half precision float) EEE 754-2008 (custom implementation)
	float float16(string endianOverride)
	{
		// get endian override
		string localEndian;
		if (endianOverride == "none") localEndian = endian;
		else localEndian = endianOverride;

		double a{};
		uint16_t i{};
		char* bytes = new char[sizeof(a)];
		double Mantissa = 0;
		int bit;
		if (file.is_open())
		{
			//file.read((char*)&i, sizeof(i)); // read int16
			file.read(bytes, sizeof(i));
			if (localEndian == "big" or localEndian == "Big") bytes = SwapEndian(bytes, sizeof(i));
			memcpy(&i, bytes, sizeof(i));

			string binary = decToBinary(i);  // bin to dec

			// get sign bit
			int sign = 1;
			if (binary[0] == '1') sign = -1;

			// get exponent bits
			string exponentbits = binary.substr(1, 5); // split binary
			int exponent = std::bitset<8>(exponentbits).to_ulong(); // convert to decimal
			exponent -= 15; // normalize (remove bias)

			// get fraction bits
			string fractionbits = binary.substr(6, 16); // split binary
			for (int i = 0; i < fractionbits.length(); i++)
			{
				bit = fractionbits[i] - 48;
				Mantissa += bit * (pow(2, -(i + 1)));
			}
			// calculate float
			a = (Mantissa + 1) * pow(2, exponent) * sign;
		}
		return a;
	}


	// define float32 reading function (full precision float)
	float float32(string endianOverride)
	{
		// get endian override
		string localEndian;
		if (endianOverride == "none") localEndian = endian;
		else localEndian = endianOverride;

		float a{};
		char* bytes = new char[sizeof(a)];
		if (file.is_open())
		{
			file.read(bytes, sizeof(a));
			if (localEndian == "big" or localEndian == "Big") bytes = SwapEndian(bytes, sizeof(a));
			memcpy(&a, bytes, sizeof(a));
		}
		return a;
	}


	// define float64 reading function (double precision float)
	double float64(string endianOverride)
	{
		// get endian override
		string localEndian;
		if (endianOverride == "none") localEndian = endian;
		else localEndian = endianOverride;

		double a{};
		char* bytes = new char[sizeof(a)];
		if (file.is_open())
		{
			file.read(bytes, sizeof(a));
			if (localEndian == "big" or localEndian == "Big") bytes = SwapEndian(bytes, sizeof(a));
			memcpy(&a, bytes, sizeof(a));
		}
		return a;
	}
}
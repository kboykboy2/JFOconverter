#include"FileUtils.h"
#include<iostream>
using std::string;

// remove extention
string removeExt(string String)
{
	return String.substr(0, String.rfind("."));

}

// change extention
string swapExt(string String, string newExt)
{
	return removeExt(String) + newExt;
}

// get name from path
string getName(string String)
{
	return String.substr(String.rfind("\\")+1, String.size());
}

// get only file name from path without extention
string stim(string String)
{
	return removeExt(getName(String));
}
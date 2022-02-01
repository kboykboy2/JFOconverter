#pragma once
#include<string>

// remove extention
std::string removeExt(std::string String);

// change extention
std::string swapExt(std::string String, std::string newExt);

// get bane from path
std::string getName(std::string String);

// get only file name from path without extention
std::string stim(std::string String);
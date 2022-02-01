/*
Created By: kboykboy
*/
// include headers
#include "MeshTool.h"
#include "Blueprint.h"
#include "Uasset.h"
#include "iostream"
#include "BinaryUtils.h"
#include "FileUtils.h"
#include "boost/filesystem.hpp"
// use boost namespace
using namespace boost::filesystem;
// define class vectors
std::vector<std::string> blueprintClasses{ "BlueprintGeneratedClass", "WidgetBlueprintGeneratedClass", "AnimBlueprintGeneratedClass" };
std::vector<std::string> meshClasses{ "SkeletalMesh", "StaticMesh" };



/* || -- Convert Uasset Function -- || */
void convertUasset(std::string current_file, std::vector<std::string> &modifiedfiles)
{
	if (current_file.substr(current_file.find_last_of(".") + 1) == "uasset")
	{
		Uasset uasset(current_file);
		std::string Class = uasset.GetClass();
		if (uasset.IsValid())
		{
			if (in_array(Class, blueprintClasses)) {
				BP::fix_bp(swapExt(current_file, ".uexp"), uasset);
				modifiedfiles.push_back(current_file);
			}
			if (in_array(Class, meshClasses))
			{
				MeshTool::FixMesh(swapExt(current_file, ".uexp"), uasset);
				modifiedfiles.push_back(current_file);
			}
		}
	}
	if (current_file.substr(current_file.find_last_of(".") + 1) == "umap")
	{
		Uasset uasset(current_file);
		if (uasset.IsValid())
		{
			BP::fix_bp(swapExt(current_file, ".uexp"), uasset);
			modifiedfiles.push_back(current_file);
		}
	}
}



/* || -- Program Entry Point  -- || */
int main(int argc, char* argv[])    // Argc is number of paths, and argv will be an array of paths for whatever is dropped onto the exe
{
	std::vector<std::string> modifiedFile;
	for (int arg = 1; arg < argc; arg++)
	{
		std::vector<std::string> dirs{ argv[arg] };
		if (is_directory(argv[arg]))
		{
			directory_iterator end_itr;
			for (int i = 0; i < dirs.size(); i++)
			{
				std::string path = dirs[i];
				for (directory_iterator itr(path); itr != end_itr; ++itr)
				{
					if (is_regular_file(itr->path())) {
						std::string current_file = itr->path().string();
						convertUasset(current_file, modifiedFile);
					}
					if (is_directory(itr->path())) dirs.push_back(itr->path().string());
				}
			}
		}
		else if (is_regular_file(argv[arg]))
		{
			convertUasset(argv[arg], modifiedFile);
		}
	}

	std::cout << "||-- Tool Finished --||\n\n -> " << modifiedFile.size() << " Files Modified\n";
	for (int i = 0; i < modifiedFile.size(); i++) {
		std::cout << " --> " << modifiedFile[i] << '\n';
	}
	std::cout << "\n\n Press Y To Replace Original Files Or Press Enter To Exit\n\n";
	std::string input;
	input = std::cin.get();
	if (input == "y" or input == "Y") {
		for (int i = 0; i < modifiedFile.size(); i++) {
			remove(modifiedFile[i]);
			rename(modifiedFile[i] + ".New", modifiedFile[i]);
			remove(swapExt(modifiedFile[i], ".uexp"));
			rename(swapExt(modifiedFile[i], ".uexp") + ".New", swapExt(modifiedFile[i], ".uexp"));
		}
	}
	std::string path;
}
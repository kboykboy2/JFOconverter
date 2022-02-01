#include"Blueprint.h"
#include"BinaryUtils.h"
#include"FileUtils.h"
#include"Uasset.h"
using namespace BinaryUtils;
std::vector<string> lightClasses{ "PointLightComponent", "RectLightComponent", "SkyLightComponent", "SpotLightComponent", "DirectionalLightComponent" };


void BP::fix_bp(string path, Uasset uasset)
{
    /* fix uexp */
	Uint   offset = 0;
	Uint64 offsetFixed = 0;
    // loop uasset exports until we find the blueprint generated class of itself {BpName}_C
    for (int i = 0; i < uasset.header.numExports; i++)
    {
        string name = uasset.nameTable[uasset.exportTable[i].NameIndex].name;
        if (name == stim(path) + "_C" or name == stim(path))
        {
            auto Export = uasset.exportTable[i];
            offset = Export.SerialOffset;
            offsetFixed = offset + Export.SerialSize - uasset.header.fileSize;
            break;
        }
    }
    // read old uexp data
    OpenFile(path);
    int uexpsize = GetSize();
    char* data1 = read(offsetFixed - 40);
    char* data2 = read();
    char bytes[4]{ 0,0,0,0 };
    CloseFile();
    // write new uexp
    std::fstream uexp(path+".New", std::ios::out | std::ios::binary);
    uexp.write(data1, offsetFixed - 40);
    uexp.write(bytes, 4);
    uexp.write(data2, uexpsize - offsetFixed + 40);
    uexp.close();
    // free memory
    delete[] data1;
    delete[] data2;
    /* fix uasset */
    uasset.Fix(offset, 4, lightClasses);
}
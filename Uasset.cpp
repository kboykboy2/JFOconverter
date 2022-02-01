#include"Uasset.h"
#include"BinaryUtils.h"
#include"FileUtils.h"
using namespace BinaryUtils;

/*
-------- || Utility Functions || --------
_________________________________________
*/


/* ||--> Read Unreal Engine String <--|| */
Fstring Read_Fstring()
{
    Fstring str{};
    str.length = int32();
    str.String = BinaryUtils::String(str.length-1);
    skip(1);
    return str;
}




/*
-------- || Uasset Functions || --------
________________________________________
*/


/* ||--> This Function Will Deserialize The Uasset Header <--|| */
void Uasset::Header::Deserialize()
{
    magic       = read(4);    // integer : always C1 83 2A 9E
    unknown1    = int32();    // integer : always F9 FF FF FF
    padding1    = read(16);   // padding : 16 bytes padding
    fileSize    = int32();    // integer : size of the uasset
    unknown2    = int32();    // integer : always seem to be 5
    unknown3    = String(4);  // string  : folder name
    padding2    = read(4);    // padding : 4 bytes padding
    unknown4    = int8();     // integer : usually 80
    numStrings  = int32();    // integer : number of strings in the string array
    p_strings   = int32();    // integer : pointer to string array
    padding3    = read(8);    // padding : 8 byes of padding
    numExports  = int32();    // integer : number of export data blocks
    p_export    = int32();    // integer : pointer to export data
    numImports  = int32();    // integer : number of import data blocks
    p_import    = int32();    // integer : pointer to import data
    p_unknown   = int32();    // integer : pointer to unknown data(just after export data)
    padding4    = read(16);   // padding : 16 bytes of padding
    UassetGUID  = read(16);   // data    : uasset Guid
    unknown8    = int32();    // integer : usually
    unknown9    = int32();    // integer : unknown
    numStrings2 = int32();    // integer : String Count again
    unknown10   = read(36);   // padding : 36 bytes of padding
    unknown11   = int32();    // integer : unknown
    padding5    = read(4);    // padding : 4 bytes of padding
    p_unknown2  = int32();    // integer : pointer to maybe export data
    totalSize   = int32();    // integer : total size of uasset plus uexp
    padding6    = read(12);   // padding : 12 bytes of padding
    unknown12   = int32();    // integer : unknown
    unknown13   = int32();    // integer : unknown offset
}

/* This Function Will Deserialize The Uasset Name Table Object */
void Uasset::Name::Deserialize()
{
    Name::name = Read_Fstring().String; // string : name of some object in uasset
    Name::hash = int32();               // data   : name hash
}

/* This Function Will Deserialize The Uasset Name Table */
Uasset::Name* Uasset::Name::Deserialize_Table(Uint Num)
{
    Name* table = new Name[Num];
    for (int i = 0; i < Num; i++)
    {
        table[i].Deserialize();
    }
    return table;
}

/* This Function Will Deserialize The Import Table Object */
void Uasset::ImportData::Deserialize()
{
    PackageIndex = int64();  // The package to import this object from
    ClassIndex   = int64();  // The index class string of the imported object in the name table
    OwnerRef     = int32();  // The object containing this imported object
    NameIndex    = int64();  // The index of the object's name in the name table
}

/* This Function Will Deserialize The Import Table */
Uasset::ImportData* Uasset::ImportData::Deserialize_Table(Uint Num)
{
    ImportData* table = new ImportData[Num];
    for (int i = 0; i < Num; i++)
    {
        table[i].Deserialize();
    }
    return table;
}

/* This Function Will Deserialize The Export Data */
void Uasset::ExportData::Deserialize()
{
    SavedLocation  = GetLocation(); // saving location to be used when fixing the uasset
    ClassIndex     = int32();  // integer : negative integer giving the index of this class from the import table
    ParentClassRef = int32();  // integer : negative integer giving the index of an object from the import table
    OwnerRef       = int32();  // integer : negative integer giving the index of an object from the import table
    ParentExport   = int32();  // integer : index of another export object, parent ?
    NameIndex      = int32();  // integer : index of this objects name in the name table
    unknown1       = int32();  // integer : unknown
    unknown2       = int32();  // integer : unknown
    SerialSize     = int64();  // integer : size of this object in the uexp
    SerialOffset   = int64();  // integer : offset to this object in the uexp || the offset acts as if the uexp was still at the end of the uasset
    unknown        = read(60); // unknown
}

/* This Function Will Deserialize The Import Table */
Uasset::ExportData* Uasset::ExportData::Deserialize_Table(Uint Num)
{
    ExportData* table = new ExportData[Num];
    for (int i = 0; i < Num; i++)
    {
        table[i].Deserialize();
    }
    return table;
}

/* This Function Will Deserialize The Uasset */
Uasset::Uasset(string path)
{
    uassetPath = path;
    OpenFile(path);
    header.Deserialize();
    nameTable   = Name::Deserialize_Table(header.numStrings);
    importTable = ImportData::Deserialize_Table(header.numImports);
    exportTable = ExportData::Deserialize_Table(header.numExports);
    CloseFile();
}

/* helper function to check if string is in array of strings */
bool in_array(const std::string& value, const std::vector<std::string>& array)
{
    return std::find(array.begin(), array.end(), value) != array.end();
}

/* This Function Will Fix Uasset Serial Sizes and Offsets */
void Uasset::Fix(Uint64 offset, Uint64 size, std::vector<string> sizeOverwrites)
{
    // read old uasset
    OpenFile(uassetPath);
    Uint64 uassetSize = GetSize();
    char* data = read();
    CloseFile();
    // write new uasset
    std::fstream uasset(uassetPath+".New", std::ios::out | std::ios::binary);
    uasset.write(data, uassetSize);

    // add conversion marker
    uasset.seekg(32);
    uasset.write("JFO ", 4);

    // fix total size in header
    Uint64 newsize = header.totalSize + size;
    uasset.seekg(169);
    uasset.write((char*)(&newsize), 4);

    Uint64 serialoffset;
    Uint64 serialsize;
    // fix export data
    for (int i = 0; i < header.numExports; i++)
    {
        serialoffset = exportTable[i].SerialOffset;
        serialsize   = exportTable[i].SerialSize;
        // fix serial offset
        if (serialoffset > offset)
        {
            uasset.seekg(exportTable[i].SavedLocation + 36);
            Uint64 newval = serialoffset + size;
            uasset.write((char*)(&newval), 8);
        }

        // fix serial size
        string objectClass = nameTable[importTable[abs(exportTable[i].ClassIndex) - 1].NameIndex].name;
        //std::cout << objectClass << '\n';
        if (serialoffset == offset or serialoffset < offset and serialoffset + serialsize > offset or in_array(objectClass, sizeOverwrites))
        {
            uasset.seekg(exportTable[i].SavedLocation + 28);
            Uint64 newval = serialsize + size;
            uasset.write((char*)(&newval), 8);
        }
    }
    // free memory
    delete[] data;
}


/* This Function Will Get The Class Of The Uasset */
string Uasset::GetClass()
{
    string Class = "unknown";
    for (int i = 0; i < (int)header.numExports; i++)
    {
        Uasset::ExportData exportData = exportTable[i];
        string name = nameTable[exportData.NameIndex].name;
        if (name == stim(uassetPath) + "_C" or name == stim(uassetPath))
        {
            Class = nameTable[importTable[abs(exportData.ClassIndex) - 1].NameIndex].name;
            break;
        }
    }
    return Class;
}


/* This Function Will Check If The Uasset Has Already Been Modified */
bool Uasset::IsValid()
{
    if (header.unknown3 == "JFO ") return false;
    else return true;
}
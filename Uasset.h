#pragma once
#include<string>
#include<vector>
typedef unsigned int Uint;
typedef unsigned long long Uint64;
using std::string;



/*
-------- || Utility Classes || --------
_______________________________________
*/


/* ||--> Unreal Engine String <--|| */
struct Fstring
{
    Uint   length;
    string String;
};
Fstring Read_Fstring();



/*
-------- || Uasset Classes || --------
______________________________________
*/

/* ||--> Uasset Class <--|| */
class Uasset
{
    /* ||--> Uasset Header Class <--|| */
    struct Header
    {
        char*  magic{};       // integer : always C1 83 2A 9E
        int    unknown1{};    // integer : always F9 FF FF FF
        char*  padding1{};    // padding : 16 bytes padding
        Uint   fileSize{};    // integer : size of the uasset
        int    unknown2{};    // integer : always seem to be 5
        string unknown3{};    // string  : folder name
        char*  padding2{};    // padding : 4 bytes padding
        short  unknown4{};    // integer : usually 80
        Uint   numStrings{};  // integer : number of strings in the string array
        Uint   p_strings{};   // integer : pointer to string array
        char*  padding3{};    // padding : 8 byes of padding
        int    numExports{};  // integer : number of export data blocks
        Uint   p_export{};    // integer : pointer to export data
        Uint   numImports{};  // integer : number of import data blocks
        Uint   p_import{};    // integer : pointer to import data
        Uint   p_unknown{};   // integer : pointer to unknown data(just after export data)
        char*  padding4{};    // padding : 16 bytes of padding
        char*  UassetGUID{};  // data    : uasset Guid
        Uint   unknown8{};    // integer : usually
        Uint   unknown9{};    // integer : unknown
        Uint   numStrings2{}; // integer : String Count again
        char*  unknown10{};   // padding : 36 bytes of padding
        Uint   unknown11{};   // integer : unknown
        char*  padding5{};    // padding : 4 bytes of padding
        Uint   p_unknown2{};  // integer : pointer to maybe export data
        Uint   totalSize{};   // integer : total size of uasset plus uexp
        char*  padding6{};    // padding : 12 bytes of padding
        Uint   unknown12{};   // integer : unknown
        Uint   unknown13{};   // integer : unknown offset



        /* This Function Will Deserialize The Uasset Header */
        void Deserialize();
    };

    /* ||--> Uasset Name Table Object Class <--|| */
    struct Name
    {
        string name;
        int    hash;



        /* This Function Will Deserialize The Uasset Name Table Object Class */
        void Deserialize();
        /* This Function Will Deserialize The Name Table */
        static Name* Deserialize_Table(Uint Num);
    };

    /* ||--> Uasset Import Table Object Class <--|| */
    struct ImportData
    {
        Uint64 PackageIndex;  // The package to import this object from
        Uint64 ClassIndex;    // The index class string of the imported object in the name table
        Uint   OwnerRef;      // The object containing this imported object
        Uint64 NameIndex;     // The index of the object's name in the name table



        /* This Function Will Deserialize The Uasset Import Data */
        void Deserialize();
        /* This Function Will Deserialize The Import Table */
        static ImportData* Deserialize_Table(Uint Num);
    };

    /* ||--> Uasset Export Table Object Class <--|| */
    struct ExportData
    {
        Uint64 SavedLocation;  // saving location to be used when fixing the uasset
        int    ClassIndex;     // integer : negative integer giving the index of this class from the import table
        int    ParentClassRef; // integer : negative integer giving the index of an object from the import table
        int    OwnerRef;       // integer : negative integer giving the index of an object from the import table
        Uint   ParentExport;   // integer : index of another export object, parent ?
        Uint   NameIndex;      // integer : index of this objects name in the name table
        int    unknown1;       // integer : unknown
        int    unknown2;       // integer : unknown
        Uint64 SerialSize;     // integer : size of this object in the uexp
        Uint64 SerialOffset;   // integer : offset to this object in the uexp || the offset acts as if the uexp was still at the end of the uasset
        char*  unknown;        // unknown



        /* This Function Will Deserialize The Export Data */
        void Deserialize();
        /* This Function Will Deserialize The Export Table */
        static ExportData* Deserialize_Table(Uint Num);
    };
    string uassetPath;
public:
    Header      header;
    Name*       nameTable;
    ImportData* importTable;
    ExportData* exportTable;



    /* This Function Will Deserialize The Uasset */
    Uasset(string path);
    /* This Function Will Fix Uasset Serial Sizes and Offsets */
    void Fix(Uint64 offset, Uint64 size, std::vector<string> sizeOverwrites = {});
    /* This Function Will Check If The Uasset Has Already Been Modified */
    bool IsValid();
    /* This Function Will Get The Class Of The Uasset */
    string GetClass();
};


/* helper function to check if string is in array of strings */
bool in_array(const std::string& value, const std::vector<std::string>& array);
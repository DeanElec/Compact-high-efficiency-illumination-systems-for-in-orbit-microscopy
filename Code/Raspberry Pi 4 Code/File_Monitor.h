#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <string>

using std::string;

string removeExtension(const string& filename);


namespace File_Monitor_Parameters {
    extern const int BUF_LEN;
    extern int fd;
    extern char buffer[];
    extern const int EVENT_SIZE;
    extern std::string Created_Image_File_Name;
}

namespace Directory_Parameters{
	extern std::string RPi_Save_Directory;
	extern std::string Confirmed_Correct_RPi_Save_Directory;
	extern std::string PC_Save_Directory;
}

class File_Monitor {
public:
    //----------------Special Member functions-----------------
    File_Monitor(std::string Directory_Path_);

    File_Monitor(const File_Monitor &F); //copy constructor

    File_Monitor &operator=(const File_Monitor &F); //equals function

    ~File_Monitor(); //destructor

    //---------------------------------------------------------

    //-----------------get functions-----------------
    std::string get_Directory_Path() const;
    //-----------------------------------------------

    //----------------set functions-------------------
    void set_Directory_Path(std::string Directory_Path);
    //------------------------------------------------

    //----------------Implementation Functions----------------
    void Monitor_Directory();

private:
    int length;
    int fd;
    int wd;
    std::string Directory_Path;
};

#endif


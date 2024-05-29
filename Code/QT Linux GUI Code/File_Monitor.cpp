#include "File_Monitor.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/inotify.h>
#include <thread>
#include <string.h>

using namespace std;

namespace File_Monitor_Parameters {
    const int BUF_LEN = 1024;
    int fd = inotify_init();
    char buffer[BUF_LEN];
    const int EVENT_SIZE = sizeof(struct inotify_event);
    std::string Created_Image_File_Name;
}

//------------------------Constructor definitions--------------------------
File_Monitor::File_Monitor(std::string Directory_Path_)
    : Directory_Path(Directory_Path_) {
    wd = inotify_add_watch(File_Monitor_Parameters::fd, get_Directory_Path().c_str(), IN_CREATE);
}

//-------------------------------------------------------------------------

//--------------------------------copy constructor-------------------------
File_Monitor::File_Monitor(const File_Monitor &FM)
    : Directory_Path(FM.Directory_Path) {}

//-------------------------------------------------------------------------

//-----------------------equals function-------------------------------------
File_Monitor &File_Monitor::operator=(const File_Monitor &FM) {
    if (this == &FM)
        return (*this); // returning reference to itself

    Directory_Path = FM.Directory_Path;

    return (*this);
}
//---------------------------------------------------------------------------

//-------------------get function definitions--------------
std::string File_Monitor::get_Directory_Path() const {
    return Directory_Path;
}
//---------------------------------------------------------

//-------------------set function definitions--------------
void File_Monitor::set_Directory_Path(std::string _Directory_Path) {
    Directory_Path = _Directory_Path;
}
//---------------------------------------------------------

//-------------------Destructor definition--------------
File_Monitor::~File_Monitor() {
    // Do cleanup for the File_Monitor class
    (void)inotify_rm_watch(fd, wd);
    (void)close(fd);
}
//---------------------------------------------------------

//-------------------Monitor_Directory implementation--------------
void File_Monitor::Monitor_Directory() {
    while (true) { // Loop forever
        std::cout << "here" << std::endl;
        length = read(File_Monitor_Parameters::fd, File_Monitor_Parameters::buffer,
                      File_Monitor_Parameters::BUF_LEN);

        if (length < 0) {
            perror("read");
        }

        int i = 0; // Reset i at the start of each loop
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&File_Monitor_Parameters::buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (!(event->mask & IN_ISDIR)) {
                        char *dot = strrchr(event->name, '.');
                        if (dot && !strcmp(dot, ".dng")) {
                        	File_Monitor_Parameters::Created_Image_File_Name = event->name;
                            std::thread t([event]{
                            File_Monitor_Parameters::Created_Image_File_Name = event->name;
                            printf("The .dng file %s was created.\n", event->name);
                            cout << "The .dng file was " << event->name << "was created" << endl;
                            cout << "value inside file mon: " << File_Monitor_Parameters::Created_Image_File_Name << endl;                             
                                
                            });
                            t.detach(); //detach the thread
                        }if(dot && !strcmp(dot, ".tiff")){
                        	cout << "tiff here" << endl;
                            //cout << "rpi save direc: " << Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory << endl;
                            //qDebug() << Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory;
                        	
                        	//-----This moves the tiff to the tiff folder
                            string Tiff_Directory_Location = Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/tiff";
                        	string File_Name_No_Extension = removeExtension(File_Monitor_Parameters::Created_Image_File_Name);
                            string Move_Tiff_To_Tiff_Folder_Command = "mv " + Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/" + File_Name_No_Extension + ".tiff" + " " + Tiff_Directory_Location;
                        	std::this_thread::sleep_for(std::chrono::milliseconds(300)); //needed to give the png time to be made before the tiff is moved
                            system(Move_Tiff_To_Tiff_Folder_Command.c_str());
                        	cout << "Tiff move cmd: " << Move_Tiff_To_Tiff_Folder_Command << endl;
                        }if(dot && !strcmp(dot, ".png")){
                        	cout << "png here" << endl;
                        	
                        	//-----This moves the png to the png folder-----            	
                            string Png_Directory_Location = Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/png";
                        	string File_Name_No_Extension = removeExtension(File_Monitor_Parameters::Created_Image_File_Name);                        	
                            string Move_Png_To_Png_Folder_Command = "mv " + Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/" + File_Name_No_Extension + ".png" + " " + Png_Directory_Location;
                            system(Move_Png_To_Png_Folder_Command.c_str());
                        	//----------------------------------------------
                        	
                        	//-----This moves the dng to the dng folder-----
                        	//the dng had to be moved here instead of in the dng if statment as the tiff and png had to be generated 1st before the dng could move other wise 
                        	//the python functions would throw exceptions as they directory they thought the image was in isnt there any more.
                            string Dng_Directory_Location = Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/dng";
                            string Move_Dng_To_Tiff_Folder_Command = "mv " + Directory_Parameters::Confirmed_Correct_Linux_PC_Save_Directory.toStdString() + "/" + File_Name_No_Extension + ".dng" + " " + Dng_Directory_Location;
                            system(Move_Dng_To_Tiff_Folder_Command.c_str());
                        	cout << "Dng move cmd: " << Move_Dng_To_Tiff_Folder_Command << endl;
                        	//----------------------------------------------
                        }
                    }
                }
            }
            i += File_Monitor_Parameters::EVENT_SIZE + event->len;
        }
    }
}
//---------------------------------------------------------






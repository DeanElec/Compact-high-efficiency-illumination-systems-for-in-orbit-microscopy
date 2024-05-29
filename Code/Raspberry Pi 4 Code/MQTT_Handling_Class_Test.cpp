//----------For PIGPIO Code----------
#include <iostream>
#include <pigpio.h>
//-----------------------------------


//----------For MQTT Code----------		
#include <mosquitto.h>
#include <stdio.h>
#include <errno.h>
#include <chrono>
#include <string.h>
#include <cstdlib>
//---------------------------------


//-----Personally Made Libraries-----
#include "MQTT_Publisher.h"
#include "IPAddress.h"
#include "DateTime.h"
#include "File_Monitor.h"
#include "Socket_Comms.h"
#include "PWM_Control.h"
//------------------------------------


//-----includes for file monitoring-----
#include <stdlib.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <thread>
//--------------------------------------

//-----additional needed libraries-----
#include <string>
#include <Python.h>
#include <map>
#include <fcntl.h> // for fcntl
#include <errno.h> // for errno, EAGAIN, EWOULDBLOCK
//-------------------------------------

#include <sstream>
#include <vector>

#include <filesystem>


#include <csignal>
#include <atomic>

using namespace std;

namespace Updated_Advanced_Image_Processing_Options_Parameters{
	string Custom_Brightness_Value_Updated;
	string Custom_White_Balance_Values_Updated;
	string Custom_Gamma_Values_Updated;
}


namespace Directory_Parameters{
	string RPi_Save_Directory;
	string Confirmed_Correct_RPi_Save_Directory;
	string PC_Save_Directory;
}

namespace Linux_PC_SCP_Parameters{
	string Linux_PC_WhoAmI;
	string Linux_PC_IP;
}

namespace PWM_Parameters{
	int Cyan_LED_PWM_Pin;
	int Red_LED_PWM_Pin;
	int Green_LED_PWM_Pin;
	int Blue_LED_PWM_Pin;
	
	int PWM_Frequency;
	
	int Cyan_LED_Brightness;
	int Red_LED_Brightness;
	int Green_LED_Brightness;
	int Blue_LED_Brightness;
}


std::atomic<bool> keepRunning(true);//atomic flag needed to control the program loop as cant ctrl + c out without
								    //this and Handle_Signal()

//signal handler function
void Handle_Signal(int signal) {
    cout << "Caught signal " << signal << ", stopping..." << endl;;
    keepRunning = false;
}



string removeExtension(const string& filename) {
    //find the last occurrence of '.' as this is the start of the file extension
    size_t Last_Dot = filename.find_last_of(".");
    if (Last_Dot == string::npos) return filename; //if somehow no extension found
    //remove from the last dot to the end of the string
    return filename.substr(0, Last_Dot);
}





std::map<string, string> Post_Processing_Parameters_Map;

map<int, pair<string, string>> selectionToParameters = {
    {0, {"output_bps", "16"}},
    {1, {"half_size", "False"}},
    {2, {"no_auto_bright", "True"}},
    {3, {"no_auto_scale", "False"}},
    {4, {"use_camera_wb", "True"}},
    {5, {"output_color", "rawpy.ColorSpace.raw"}},
    {6, {"demosaic_algorithm", "rawpy.DemosaicAlgorithm.AHD"}},
    {7, {"demosaic_algorithm", "rawpy.DemosaicAlgorithm.AAHD"}},
    {8, {"demosaic_algorithm", "rawpy.DemosaicAlgorithm.VNG"}},
    {9, {"demosaic_algorithm", "rawpy.DemosaicAlgorithm.PPG"}},
    
    {10, {"bright", Updated_Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value_Updated}},
    {11, {"user_wb", Updated_Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Values_Updated}},
    {12, {"gamma", Updated_Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Values_Updated}},
    // Add additional mappings as needed
};



void sendIPAddressAndAwaitConfirmation() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //create a UDP socket for IPV4 comms
    if (sock < 0) {
        cerr << "Error creating socket." << endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; //buffer size set for IPv4 addresses
    serverAddr.sin_port = htons(8080); //set the port
    inet_pton(AF_INET, "224.0.0.251", &serverAddr.sin_addr); //place holder address set here

    
    fcntl(sock, F_SETFL, O_NONBLOCK); //make the socket non-blocking

    FILE* ipFile = popen("hostname -I", "r"); //gets the pi IP address cant use system() as doesnt allow to store the output
    if (ipFile == NULL) {
        cerr << "Error opening file." << endl;
        close(sock);
        return;
    }
	
//------reads the ip address into ipAddress------
    char ipAddress[16];
    if (fscanf(ipFile, "%s", ipAddress) == 0) {
        cerr << "Failed to read IP address." << endl;
        pclose(ipFile);
        close(sock);
        return;
    }
    pclose(ipFile);
//---------------------------------------------


    cout << "Raspberry Pi's IP address: " << ipAddress << endl;

//-----variables needed for waiting for confirmation loop-----
    bool confirmed = false;
    char buffer[1024];
    sockaddr_in fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
//------------------------------------------------------------


    while (!confirmed) { //while confirmed is false continue to loop
        // Send the IP address to the Linux PC
        sendto(sock, ipAddress, strlen(ipAddress), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        //wait for a half a second before checking for a response
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //check for a confirmation message from the Linux PC
        ssize_t respLength = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&fromAddr, &fromAddrLen);
        if (respLength > 0) {
            buffer[respLength] = '\0';  //add a null terminate to the received string
            cout << "Received response from Linux PC: " << buffer << endl;
            confirmed = true;
        } else {
            cout << "No response received, retrying..." << endl;
        }
    }

    close(sock); //close the socket once everything is finished
}



File_Monitor* file_Monitor = nullptr;
std::thread monitor_files_thread;

void runMonitoringThread() {
    file_Monitor = new File_Monitor(Directory_Parameters::RPi_Save_Directory);
    file_Monitor->Monitor_Directory(); 
}


int Raw_Image_Reconstruction(const char* filename) {
    cout << "In recon func" << endl;
    if (!Py_IsInitialized()) {
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('.')");
    }

    PyObject* pModule = PyImport_ImportModule("image_processing");
    if (!pModule) {
        PyErr_Print();
        cerr << "Failed to import\n";
        return -1;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, "process_image");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject* pDict = PyDict_New();
        for (const auto& pair : Post_Processing_Parameters_Map) {
            PyObject* pKey = PyUnicode_FromString(pair.first.c_str());
            PyObject* pValue = PyUnicode_FromString(pair.second.c_str());
            PyDict_SetItem(pDict, pKey, pValue);
            Py_DECREF(pKey);
            Py_DECREF(pValue);
        }

        PyObject* pArgs = PyTuple_Pack(2, PyUnicode_FromString(filename), pDict);
        PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
        if (!pResult) PyErr_Print();
        Py_XDECREF(pArgs);
        Py_XDECREF(pDict);
        Py_XDECREF(pResult);
    } else {
        PyErr_Print();
        cerr << "Failed to call process_image function\n";
    }

    Py_XDECREF(pFunc);
    Py_XDECREF(pModule);

    return 0;
} 

//called after the client has connected to the broker
void on_connect(struct mosquitto *mosq,	//pointer to our clients object
		void *obj, // pointer to clients ID
		int rc) // return code of connect function (error codes)
		{
			
	printf("ID: %d\n", * (int *) obj);
	
//----------------------Error Checking----------------------
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
//----------------------------------------------------------


//-----------subscribe if connection was successful------------

	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Image_Format", //Topic to subscribe to				
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "RPI_Save_Directory", //Topic to subscribe to
			     0); //quality of service level (QoS)
			
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Image_Processing_Options", //Topic to subscribe to
			     0); //quality of service level (QoS)

	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Take_Image", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "PC_Save_Directory", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Advanced_Image_Processing_Options", //Topic to subscribe to
			     0); //quality of service level (QoS)
			    
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Cyan_LED_Brightness_Value", //Topic to subscribe to
			     0); //quality of service level (QoS)
			 
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Red_LED_Brightness_Value", //Topic to subscribe to
			     0); //quality of service level (QoS)
			  
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Green_LED_Brightness_Value", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Blue_LED_Brightness_Value", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "PWM_Frequency", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "WhoAmI", //Topic to subscribe to
			     0); //quality of service level (QoS)
			    
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Linux_PC_IP", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "Program_Closed", //Topic to subscribe to
			     0); //quality of service level (QoS)
			     
	mosquitto_subscribe(mosq,  //Clients Object
			    NULL,  //Message ID	
			    "PWM_Pins", //Topic to subscribe to
			     0); //quality of service level (QoS)

}
//-------------------------------------------------------------


struct MQTTHandler {
    int Image_Format_Value;
    int Image_Processing_Options_Value;
    string RPI_Save_Directory_Value;
};

MQTTHandler test;



//--------------------MQTT on recieving message from subscribe--------------------
inline void on_message(struct mosquitto *mosq, //Pointer to our Clients Object
	 	void *obj, //Pointer to message ID
	 	const struct mosquitto_message *msg) // Struct containging message data
{	


		printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload); //Print the recieved message
	
	if(!strcmp(msg->topic,"Image_Format"))//this compares the string msg->topic to HV_Topic and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Image_Format" << endl;
		test.Image_Format_Value = atoi((char *) msg->payload); // convert char* payload to integer value
		
		cout << test.Image_Format_Value << endl;
		
		
	}
	
	
	if(!strcmp(msg->topic,"RPI_Save_Directory"))//this compares the string msg->topic to HV_Topic and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : RPI_Save_Directory" << endl;
		
	    Directory_Parameters::RPi_Save_Directory = (char *) msg->payload;
	
		
    		std::cout << "RPi directory: " << Directory_Parameters::RPi_Save_Directory << std::endl;
		
    		std::filesystem::path dirPath(Directory_Parameters::RPi_Save_Directory);
		
    		//check if the path is an absolute path and either doesnt exist or already exists
    		if (dirPath.is_absolute()) {
    		    if (!std::filesystem::exists(dirPath)) {
    		        //path is absolute but does not exist so create it
    		        if (std::filesystem::create_directories(dirPath)) {
    		            std::cout << "The directory was successfully created." << std::endl;
		
    		            Directory_Parameters::Confirmed_Correct_RPi_Save_Directory = Directory_Parameters::RPi_Save_Directory;
		
    		            //setup a thread for monitoring files
    		            std::thread monitor_files_thread(runMonitoringThread);
    		            monitor_files_thread.detach(); //detach the thread to allow it to run independently of main thread
    		        }
    		    } else {
    		        // Here the path is absolute and already exists on the system.
    		        std::cout << "The path already exists on the system." << std::endl;
		
    		        Directory_Parameters::Confirmed_Correct_RPi_Save_Directory = Directory_Parameters::RPi_Save_Directory;
		
    		        //setup a thread for monitoring files.
    		        std::thread monitor_files_thread(runMonitoringThread);
    		        monitor_files_thread.detach(); //detach the thread to allow it to run independently of main thread
    		    }
		
    		    //subdirectory names
    		    std::string subdirs[] = {"tiff", "dng", "png"};
		
    		    //check and create subdirectories if they don't exist, inside the absolute path the user entered
    		    for (const std::string& subdir : subdirs) {
    		        std::filesystem::path subdirPath = dirPath / subdir;
    		        if (!std::filesystem::exists(subdirPath)) {
    		            if (std::filesystem::create_directory(subdirPath)) {
    		                std::cout << "Subdirectory for " << subdir << " was successfully created." << std::endl;
    		            }
    		        } else {
    		            std::cout << "Subdirectory for " << subdir << " already exists." << std::endl;
    		        }
    		    }
    		} else {
    		    // The path is not absolute, thus is invalid.
    		    std::cout << "This is an invalid path. Please provide a full, absolute path." << std::endl;
    		}

}

	
	
	if(!strcmp(msg->topic,"Image_Processing_Options"))//this compares the string msg->topic to HV_Topic and returns 0 if true thus the !strcmp
	{

        cout << "message on topic: Image_Processing_Options" << endl;

        string Indexed_User_Selected_Processing_Options = (char *)msg->payload;
        std::vector<int> User_Selected_Indexed_Processing_Options;
        std::stringstream ss(Indexed_User_Selected_Processing_Options); 
        string Temporary_Holder;

        while (std::getline(ss, Temporary_Holder, ',')) {
            User_Selected_Indexed_Processing_Options.push_back(std::stoi(Temporary_Holder));
        }

        Post_Processing_Parameters_Map.clear();

        for (int selection : User_Selected_Indexed_Processing_Options) {
            auto it = selectionToParameters.find(selection);
            if (it != selectionToParameters.end()) {
                Post_Processing_Parameters_Map[it->second.first] = it->second.second;
            } else {
                cout << "Selection not recognized: " << selection << endl;
            }
        }

        for (const auto& pair : Post_Processing_Parameters_Map) {
            cout << "Key: " << pair.first << ", Value: " << pair.second << endl;
        }


		
}




	if(!strcmp(msg->topic,"Take_Image"))//this compares the string msg->topic to Take_Image and returns 0 if true thus the !strcmp
	{

		//cout << "message on topic : Take_Image" << endl;
		//int Recived_Take_Image_Value = atoi((char *) msg->payload); // convert char* payload to integer value
		//cout << "Recived Value: " << Recived_Take_Image_Value << endl;
		MQTT_Publisher publisher("publisher-test", IPAddress::getInstance().getAddress(), 1883);
		publisher.publish("Take_Image", "Image Taken Now");
		
		//cout << test.Image_Format_Value << endl;

		//decision made to not put system in to prevent raw reconstruction from being called while
		//processing an image, as a delay will be added to GUI to only allow button to be pressed
		//every X seconds
	
		
		string Current_Time = DateTime::getInstance().getDateTime();

		string command = "libcamera-still -t 5 -r -o " + Directory_Parameters::Confirmed_Correct_RPi_Save_Directory + "/" + "Image_" + Current_Time + ".dng"; //-t 5 means wait 5ms -r means take raw image
		
		
		
		//cout << command << endl;
		
		system(command.c_str()); 
		
		
		string Path_for_Image_Processing = Directory_Parameters::Confirmed_Correct_RPi_Save_Directory + "/" + File_Monitor_Parameters::Created_Image_File_Name;
		
		cout << "im process path" << Path_for_Image_Processing << endl;
		
		//cout << "File Name "<< File_Monitor_Parameters::Created_Image_File_Name <<endl;
		
		Raw_Image_Reconstruction(Path_for_Image_Processing.c_str());
		
		
		
		string File_Type_Sub_Directories[] = {"/dng", "/tiff", "/png"};
		string File_Extension_Types[] = {".dng", ".tiff", ".png"};
		string SCP_Full_Path_To_All_Sub_Directories[3];
		string File_Transfer_Command = "";
		string File_Name_No_Extension = "";
		File_Name_No_Extension = removeExtension(File_Monitor_Parameters::Created_Image_File_Name);
		
		
		cout << "----------------------here---------------------"<< endl;
		
		
//-------------------populates an array with correct directory paths and sends the files over scp-------------------
		for(uint8_t i = 0; i<= 2; ++i){//dont know why this didnt work inside just one for loop cause with just 1 the scp commands didnt execute
			SCP_Full_Path_To_All_Sub_Directories[i] = Directory_Parameters::Confirmed_Correct_RPi_Save_Directory + File_Type_Sub_Directories[i];		
		}
		
		for(uint8_t i = 0; i<= 2; ++i){
			//replace /home/ with the location of the SSH key, also not required to have StrictHostKeyChecking = no
			File_Transfer_Command = "scp -o StrictHostKeyChecking=no -i /home/ " + SCP_Full_Path_To_All_Sub_Directories[i] + "/" + File_Name_No_Extension + File_Extension_Types[i] + " " + Linux_PC_SCP_Parameters::Linux_PC_WhoAmI +  "@"  + Linux_PC_SCP_Parameters::Linux_PC_IP + ":" + Directory_Parameters::PC_Save_Directory;
			
			cout << "File trans cmd: " << File_Transfer_Command << endl;
			system(File_Transfer_Command.c_str());
		}
//-----------------------------------------------------------------------------------------------------------------
			
}
	
		
	if(!strcmp(msg->topic,"PC_Save_Directory"))//this compares the string msg->topic to PC_Save_Directory and returns 0 if true thus the !strcmp
	{
		cout << "message on topic : PC_Save_Directory" << endl;
	    Directory_Parameters::PC_Save_Directory = (char *) msg->payload;
	}
	

	if(!strcmp(msg->topic,"Advanced_Image_Processing_Options")) { //this compares the string msg->topic to Advanced_Image_Processing_Options and returns 0 if true thus the !strcmp
		cout << "Message on topic: Advanced_Image_Processing_Options" << endl;
  		string Received_Advanced_Processing_Options = (char *)msg->payload; //stores all the advanced processing options with semi colon delimiter

		//these local variable copies were needed as without them when just assigning values to the namespace variables
    	//it would have to run twice to see the user input because by default the namespace values are null, so these are
    	//used to get the value then assign to the namespace so it always contains up to date data, not ideal but is a work around.
 	 	string Custom_Brightness_Value;
    	string Custom_White_Balance_Value;
    	string Custom_Gamma_Value;

    	std::stringstream ss(Received_Advanced_Processing_Options);
    	string segment;
    	int segmentCount = 0;

    	//try to read each segment from the stringstream
    	while(std::getline(ss, segment, ';')) {
        	if(segmentCount == 0) Custom_Brightness_Value = segment;
        	else if(segmentCount == 1) Custom_White_Balance_Value = segment;
        	else if(segmentCount == 2) Custom_Gamma_Value = segment;
        	++segmentCount;
    	}

//---------------only update the map and variables if the values have been received---------------
    	if(!Custom_Brightness_Value.empty()) {
        	Post_Processing_Parameters_Map["bright"] = Custom_Brightness_Value;
        	Updated_Advanced_Image_Processing_Options_Parameters::Custom_Brightness_Value_Updated = Custom_Brightness_Value;
        	cout << "Brightness: " << Custom_Brightness_Value << endl;
    	}

    	if(!Custom_White_Balance_Value.empty()) {
        	Post_Processing_Parameters_Map["user_wb"] = Custom_White_Balance_Value;
        	Updated_Advanced_Image_Processing_Options_Parameters::Custom_White_Balance_Values_Updated = Custom_White_Balance_Value;
        	cout << "user_wb: " << Custom_White_Balance_Value << endl;
    	}

    	if(!Custom_Gamma_Value.empty()) {
        	Post_Processing_Parameters_Map["gamma"] = Custom_Gamma_Value;
        	Updated_Advanced_Image_Processing_Options_Parameters::Custom_Gamma_Values_Updated = Custom_Gamma_Value;
        	cout << "gamma: " << Custom_Gamma_Value << endl;
    	}
//these were needed as otherwise the map that gets converted to a dictionary has parameters
//with empty strings and would cause exceptions to occur inside of the function that calls
//the python functions that were compiled into C
//------------------------------------------------------------------------------------------------
    
    
}
	
	
	
	if(!strcmp(msg->topic,"Cyan_LED_Brightness_Value"))//this compares the string msg->topic to Cyan_LED_Brightness_Value and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Cyan_LED_Brightness_Value" << endl;
		
		
	    int Cyan_LED_Brightness_Value = atoi((char *) msg->payload); //convert value to int	
	    PWM_Parameters::Cyan_LED_Brightness = Cyan_LED_Brightness_Value;
	    
	    //cout << "Cyan Brightness int: " << Cyan_LED_Brightness_Value << endl;
	    
	    cout << "Cyan Pin: " << PWM_Parameters::Cyan_LED_PWM_Pin << endl;
	    cout << "Red Pin: " << PWM_Parameters::Red_LED_PWM_Pin << endl;
	    cout << "Green Pin: " << PWM_Parameters::Green_LED_PWM_Pin << endl;
	    cout << "Blue Pin: " << PWM_Parameters::Blue_LED_PWM_Pin << endl;
	    cout << "Cyan Brightness: " << PWM_Parameters::Cyan_LED_Brightness << endl;
	    cout << "Red Brightness: " << PWM_Parameters::Red_LED_Brightness << endl;
	    cout << "Green Brightness: " << PWM_Parameters::Green_LED_Brightness << endl;
	    cout << "Blue Brightness: " << PWM_Parameters::Blue_LED_Brightness << endl;
	    cout << "PWM Frequency: " << PWM_Parameters::PWM_Frequency << endl;
	    
	    PWM_Control Cyan_Control(PWM_Parameters::Cyan_LED_PWM_Pin,
	    						 PWM_Parameters::Red_LED_PWM_Pin,
	    						 PWM_Parameters::Green_LED_PWM_Pin,
	    						 PWM_Parameters::Blue_LED_PWM_Pin,
	    						 PWM_Parameters::Cyan_LED_Brightness,
	    						 PWM_Parameters::Red_LED_Brightness,
	    						 PWM_Parameters::Green_LED_Brightness,
	    						 PWM_Parameters::Blue_LED_Brightness,
	    						 PWM_Parameters::PWM_Frequency);   
	}
	
	
	if(!strcmp(msg->topic,"Red_LED_Brightness_Value"))//this compares the string msg->topic to Red_LED_Brightness_Value and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Red_LED_Brightness_Value" << endl;
		
		
	    int Red_LED_Brightness_Value = atoi((char *) msg->payload); //convert value to int
	    PWM_Parameters::Red_LED_Brightness = Red_LED_Brightness_Value;
	    cout << "Red Brightness int: " << Red_LED_Brightness_Value << endl;
	    
	    
	    cout << "Cyan Pin: " << PWM_Parameters::Cyan_LED_PWM_Pin << endl;
	    cout << "Red Pin: " << PWM_Parameters::Red_LED_PWM_Pin << endl;
	    cout << "Green Pin: " << PWM_Parameters::Green_LED_PWM_Pin << endl;
	    cout << "Blue Pin: " << PWM_Parameters::Blue_LED_PWM_Pin << endl;
	    cout << "Cyan Brightness: " << PWM_Parameters::Cyan_LED_Brightness << endl;
	    cout << "Red Brightness: " << PWM_Parameters::Red_LED_Brightness << endl;
	    cout << "Green Brightness: " << PWM_Parameters::Green_LED_Brightness << endl;
	    cout << "Blue Brightness: " << PWM_Parameters::Blue_LED_Brightness << endl;
	    cout << "PWM Frequency: " << PWM_Parameters::PWM_Frequency << endl;
	    
	    
	    PWM_Control Red_Control(PWM_Parameters::Cyan_LED_PWM_Pin,
	    						PWM_Parameters::Red_LED_PWM_Pin,
	    						PWM_Parameters::Green_LED_PWM_Pin,
	    						PWM_Parameters::Blue_LED_PWM_Pin,
	    						PWM_Parameters::Cyan_LED_Brightness,
	    						PWM_Parameters::Red_LED_Brightness,
	    						PWM_Parameters::Green_LED_Brightness,
	    						PWM_Parameters::Blue_LED_Brightness,
	    						PWM_Parameters::PWM_Frequency);
	}


	if(!strcmp(msg->topic,"Green_LED_Brightness_Value"))//this compares the string msg->topic to Green_LED_Brightness_Value and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Green_LED_Brightness_Value" << endl;
		
		
	    int Green_LED_Brightness_Value = atoi((char *) msg->payload); //convert value to int
	    PWM_Parameters::Green_LED_Brightness = Green_LED_Brightness_Value;
		cout << "Green Brightness int: " << Green_LED_Brightness_Value << endl;
		
	    cout << "Cyan Pin: " << PWM_Parameters::Cyan_LED_PWM_Pin << endl;
	    cout << "Red Pin: " << PWM_Parameters::Red_LED_PWM_Pin << endl;
	    cout << "Green Pin: " << PWM_Parameters::Green_LED_PWM_Pin << endl;
	    cout << "Blue Pin: " << PWM_Parameters::Blue_LED_PWM_Pin << endl;
	    cout << "Cyan Brightness: " << PWM_Parameters::Cyan_LED_Brightness << endl;
	    cout << "Red Brightness: " << PWM_Parameters::Red_LED_Brightness << endl;
	    cout << "Green Brightness: " << PWM_Parameters::Green_LED_Brightness << endl;
	    cout << "Blue Brightness: " << PWM_Parameters::Blue_LED_Brightness << endl;
	    cout << "PWM Frequency: " << PWM_Parameters::PWM_Frequency << endl;
		
		
	    PWM_Control Green_Control(PWM_Parameters::Cyan_LED_PWM_Pin,
	    						  PWM_Parameters::Red_LED_PWM_Pin,
	    						  PWM_Parameters::Green_LED_PWM_Pin,
	    						  PWM_Parameters::Blue_LED_PWM_Pin,
	    						  PWM_Parameters::Cyan_LED_Brightness,
	    						  PWM_Parameters::Red_LED_Brightness,
	    						  PWM_Parameters::Green_LED_Brightness,
	    						  PWM_Parameters::Blue_LED_Brightness,
	    						  PWM_Parameters::PWM_Frequency);
	}
	
	
	if(!strcmp(msg->topic,"Blue_LED_Brightness_Value"))//this compares the string msg->topic to Blue_LED_Brightness_Value and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Blue_LED_Brightness_Value" << endl;
		
		
	    int Blue_LED_Brightness_Value = atoi((char *) msg->payload); //convert value to int
	    PWM_Parameters::Blue_LED_Brightness = Blue_LED_Brightness_Value;
	    cout << "Blue Brightness int: " << Blue_LED_Brightness_Value << endl;
	    
	    cout << "Cyan Pin: " << PWM_Parameters::Cyan_LED_PWM_Pin << endl;
	    cout << "Red Pin: " << PWM_Parameters::Red_LED_PWM_Pin << endl;
	    cout << "Green Pin: " << PWM_Parameters::Green_LED_PWM_Pin << endl;
	    cout << "Blue Pin: " << PWM_Parameters::Blue_LED_PWM_Pin << endl;
	    cout << "Cyan Brightness: " << PWM_Parameters::Cyan_LED_Brightness << endl;
	    cout << "Red Brightness: " << PWM_Parameters::Red_LED_Brightness << endl;
	    cout << "Green Brightness: " << PWM_Parameters::Green_LED_Brightness << endl;
	    cout << "Blue Brightness: " << PWM_Parameters::Blue_LED_Brightness << endl;
	    cout << "PWM Frequency: " << PWM_Parameters::PWM_Frequency << endl;
	    
	    PWM_Control Blue_Control(PWM_Parameters::Cyan_LED_PWM_Pin,
	    						 PWM_Parameters::Red_LED_PWM_Pin,
	    						 PWM_Parameters::Green_LED_PWM_Pin,
	    						 PWM_Parameters::Blue_LED_PWM_Pin,
	    						 PWM_Parameters::Cyan_LED_Brightness,
	    						 PWM_Parameters::Red_LED_Brightness,
	    						 PWM_Parameters::Green_LED_Brightness,
	    						 PWM_Parameters::Blue_LED_Brightness,
	    						 PWM_Parameters::PWM_Frequency);
	}

	
	if(!strcmp(msg->topic,"PWM_Frequency"))//this compares the string msg->topic to PC_Save_Directory and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : PWM_Frequency" << endl;
		
		
	    int PWM_Frequency = atoi((char *) msg->payload);
	    
	    PWM_Parameters::PWM_Frequency = PWM_Frequency;
	    cout << "NS: " << PWM_Parameters::PWM_Frequency << endl;
		
	}
	
	
	if(!strcmp(msg->topic,"WhoAmI"))//this compares the string msg->topic to PC_Save_Directory and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : WhoAmI" << endl;
		
		
	    Linux_PC_SCP_Parameters::Linux_PC_WhoAmI = (char *) msg->payload;
		cout << "Linux who am I: " << Linux_PC_SCP_Parameters::Linux_PC_WhoAmI << endl;
	}
	
	
	if(!strcmp(msg->topic,"Linux_PC_IP"))//this compares the string msg->topic to PC_Save_Directory and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Linux_PC_IP" << endl;
		
		
	    Linux_PC_SCP_Parameters::Linux_PC_IP = (char *) msg->payload;
		cout << "Linux PC IP" << Linux_PC_SCP_Parameters::Linux_PC_IP << endl;
	}
	
	
	if(!strcmp(msg->topic,"Program_Closed"))//this compares the string msg->topic to Program_Closed and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : Program_Closed" << endl;
		
		
	    string GUI_Closed = (char *) msg->payload;
	    cout << "GUI Closed value: " << GUI_Closed << endl;
	    
	    sendIPAddressAndAwaitConfirmation();
		
	}
	
	if(!strcmp(msg->topic,"PWM_Pins"))//this compares the string msg->topic to PWM_Pins and returns 0 if true thus the !strcmp
	{

		cout << "message on topic : PWM_Pins" << endl;
		
		
	    string PWM_Pins = (char *) msg->payload;
	    cout << "PWM Pin values: " << PWM_Pins << endl;
	    
	    
	    
	    std::vector<int> User_Selected_PWM_Pins;
        std::stringstream ss(PWM_Pins); 
        string Temporary_Holder;

        while (std::getline(ss, Temporary_Holder, ',')) {
            User_Selected_PWM_Pins.push_back(std::stoi(Temporary_Holder));
        }
	    
    	//assign the values to the namespace variables so can be accessed by else where, mainly the LED brightness topics
    	PWM_Parameters::Cyan_LED_PWM_Pin = User_Selected_PWM_Pins[0];
    	PWM_Parameters::Red_LED_PWM_Pin = User_Selected_PWM_Pins[1];
  	    PWM_Parameters::Green_LED_PWM_Pin = User_Selected_PWM_Pins[2];
  		PWM_Parameters::Blue_LED_PWM_Pin = User_Selected_PWM_Pins[3];

    	//print the assigned values for confirmation
    	cout << "Assigned PWM Pins:" << endl;
    	cout << "Cyan: " << PWM_Parameters::Cyan_LED_PWM_Pin << endl;
    	cout << "Red: " << PWM_Parameters::Red_LED_PWM_Pin << endl;
    	cout << "Green: " << PWM_Parameters::Green_LED_PWM_Pin << endl;
    	cout << "Blue: " << PWM_Parameters::Blue_LED_PWM_Pin << endl;
    		    	
	    
		
	}
	
}
//--------------------------------------------------------------------------------



class MQTT_Subscribe_Handling {
private:
    struct mosquitto *mosq;  //declare mosq as a member variable

public:
    MQTT_Subscribe_Handling() {
        mosq = nullptr;  //initialize mosq to nullptr in the constructor
    }

    ~MQTT_Subscribe_Handling() {	//destructor when needed will perform mosquitto cleanup
            mosquitto_disconnect(mosq);
            mosquitto_destroy(mosq);
        	mosquitto_lib_cleanup();
    }
    
    
    int subscribe(){

		int rc, id=1; //id is the client id 1 is an arbitrary value

		mosquitto_lib_init(); 

		struct mosquitto *mosq; //create the client object


		mosq = mosquitto_new("subscribe-test", //Clients Name
	                      	true, //Clean session selection (boolean)
	                      	&id); //pointer to clients id
	                      
	                      
		mosquitto_connect_callback_set(mosq, //Client object
	                               	on_connect); //Call to our connect function
	
	
		mosquitto_message_callback_set(mosq, //Client object
	                               	on_message); //Call to our connect function
	
	
//-----------------------Connect to the broker-----------------------
		rc = mosquitto_connect(mosq, //client object
	                       	IPAddress::getInstance().getAddress(), //ip address
	                       	1883, //port number
	                       	10); //timeout length
//-------------------------------------------------------------------

//---------------------------------Error checking--------------------
		if(rc) {
			printf("Could not connect to Broker with return code %d\n", rc);
			return -1;
		}
//-------------------------------------------------------------------

		mosquitto_loop_start(mosq); //Handles everything on the client side such as recieving of messages

		while(keepRunning.load()){};
		mosquitto_loop_stop(mosq, true);


		return 0;
	}
    
};


int main() {
	 std::signal(SIGINT, Handle_Signal);

	sendIPAddressAndAwaitConfirmation();
    
//   monitor_files_thread = std::thread(runMonitoringThread); //make a thread
//    monitor_files_thread.detach();//Detach the thread to allow it to run independently
    

    MQTT_Subscribe_Handling mqttHandling; // this could also potentially be run on its own thread.
    mqttHandling.subscribe();
    
    
    
   delete file_Monitor; 
    
    return 0;
}



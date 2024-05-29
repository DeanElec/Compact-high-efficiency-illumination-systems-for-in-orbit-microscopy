#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <pigpio.h>
#include <iostream>

class PWM_Control {
public:
//----------------Special Member functions-----------------
    PWM_Control(int Red_PWM_Pin_,
                int Green_PWM_Pin_,
                int Blue_PWM_Pin_,
                int Cyan_PWM_Pin_,
                int Red_Brightness_,
                int Green_Brightness_,
                int Blue_Brightness_,
                int Cyan_Brightness_,
                int PWM_Frequency_); //Constructor


    PWM_Control(const PWM_Control& P); //Copy Constructor
    
    PWM_Control& operator=(const PWM_Control& P); //Equals functon
    
    ~PWM_Control(){gpioTerminate();} //Destructor
//---------------------------------------------------------

//-----------------get functions-----------------
    int get_Red_Brightness();
    int get_Green_Brightness();
    int get_Blue_Brightness();
    int get_Cyan_Brightness();

    int get_Red_PWM_Pin();
    int get_Green_PWM_Pin();
    int get_Blue_PWM_Pin();
    int get_Cyan_PWM_Pin();

    int get_PWM_Frequency();
//------------------------------------------------

//----------------set functions-------------------
    void set_All_Brightnesses(int Red_Brightness,
                              int Green_Brightness,
                              int Blue_Brightness,
                              int Cyan_Brightness);

    void set_Red_Brightness(int Red_Brightness);
    void set_Green_Brightness(int Green_Brightness);
    void set_Blue_Brightness(int Blue_Brightness);
    void set_Cyan_Brightness(int Cyan_Brightness);

    void set_Red_PWM_Pin(int Red_PWM_Pin);
    void set_Green_PWM_Pin(int Green_PWM_Pin);
    void set_Blue_PWM_Pin(int Blue_PWM_Pin);
    void set_Cyan_PWM_Pin(int Cyan_PWM_Pin);

    void set_PWM_Frequency(int PWM_Frequency);
//------------------------------------------------

//----------PWM_initialisation----------
    void PWM_initialisation();
//--------------------------------------

private:
    int Red_PWM_Pin;
    int Green_PWM_Pin;
    int Blue_PWM_Pin;
    int Cyan_PWM_Pin;

    int Red_Brightness;
    int Green_Brightness;
    int Blue_Brightness;
    int Cyan_Brightness;

    int PWM_Frequency;
};

#endif


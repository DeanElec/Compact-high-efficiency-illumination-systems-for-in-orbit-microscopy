#include "PWM_Control.h"

using namespace std;
//------------------------Constructor definitions---------------------------
PWM_Control::PWM_Control(int Red_PWM_Pin_,
                         int Green_PWM_Pin_,
                         int Blue_PWM_Pin_,
                         int Cyan_PWM_Pin_,
                         int Red_Brightness_,
                         int Green_Brightness_,
                         int Blue_Brightness_,
                         int Cyan_Brightness_,
                         int PWM_Frequency_)
    : Red_PWM_Pin(Red_PWM_Pin_),
      Green_PWM_Pin(Green_PWM_Pin_),
      Blue_PWM_Pin(Blue_PWM_Pin_),
      Cyan_PWM_Pin(Cyan_PWM_Pin_),
      Red_Brightness(Red_Brightness_),
      Green_Brightness(Green_Brightness_),
      Blue_Brightness(Blue_Brightness_),
      Cyan_Brightness(Cyan_Brightness_),
      PWM_Frequency(PWM_Frequency_) {}
//-------------------------------------------------------------------------

//------------------------------copy constructor----------------------
PWM_Control::PWM_Control(const PWM_Control& PWM)
    : Red_PWM_Pin(PWM.Red_PWM_Pin),
      Green_PWM_Pin(PWM.Green_PWM_Pin),
      Blue_PWM_Pin(PWM.Blue_PWM_Pin),
      Cyan_PWM_Pin(PWM.Cyan_PWM_Pin),
      Red_Brightness(PWM.Red_Brightness),
      Green_Brightness(PWM.Green_Brightness),
      Blue_Brightness(PWM.Blue_Brightness),
      Cyan_Brightness(PWM.Cyan_Brightness),
      PWM_Frequency(PWM.PWM_Frequency) {}
//----------------------------------------------------------------------


//-----------------------equals function-------------------------------------
PWM_Control& PWM_Control::operator=(const PWM_Control& PWM) {
    if (this == &PWM)
        return (*this);

    Red_PWM_Pin = PWM.Red_PWM_Pin;
    Green_PWM_Pin = PWM.Green_PWM_Pin;
    Blue_PWM_Pin = PWM.Blue_PWM_Pin;
    Cyan_PWM_Pin = PWM.Cyan_PWM_Pin;

    Red_Brightness = PWM.Red_Brightness;
    Green_Brightness = PWM.Green_Brightness;
    Blue_Brightness = PWM.Blue_Brightness;
    Cyan_Brightness = PWM.Cyan_Brightness;

    PWM_Frequency = PWM.PWM_Frequency;

    return (*this);
}
//---------------------------------------------------------------------------


//------------------------get function definitions-------------------
int PWM_Control::get_Red_Brightness() { return Red_Brightness; }
int PWM_Control::get_Green_Brightness() { return Green_Brightness; }
int PWM_Control::get_Blue_Brightness() { return Blue_Brightness; }
int PWM_Control::get_Cyan_Brightness() { return Cyan_Brightness; }

int PWM_Control::get_Red_PWM_Pin() { return Red_PWM_Pin; }
int PWM_Control::get_Green_PWM_Pin() { return Green_PWM_Pin; }
int PWM_Control::get_Blue_PWM_Pin() { return Blue_PWM_Pin; }
int PWM_Control::get_Cyan_PWM_Pin() { return Cyan_PWM_Pin; }

int PWM_Control::get_PWM_Frequency() { return PWM_Frequency; }


//--------------------------------------------------------------------

void PWM_Control::set_All_Brightnesses(int _Red_Brightness, int _Green_Brightness, int _Blue_Brightness, int _Cyan_Brightness){
    if (_Red_Brightness >= 0 && _Red_Brightness <= 100) { //100 different values for duty cycle control
        Red_Brightness = _Red_Brightness;
        gpioPWM(get_Red_PWM_Pin(), Red_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
    
    if (_Green_Brightness >= 0 && _Green_Brightness <= 100) { //100 different values for duty cycle control
        Green_Brightness = _Green_Brightness;
        gpioPWM(get_Green_PWM_Pin(), Green_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    } 
    
    if (_Blue_Brightness >= 0 && _Blue_Brightness <= 100) { //100 different values for duty cycle control
        Blue_Brightness = _Blue_Brightness;
        gpioPWM(get_Blue_PWM_Pin(), Blue_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
    
    if (_Cyan_Brightness >= 0 && _Cyan_Brightness <= 100) { //100 different values for duty cycle control
        Cyan_Brightness = _Cyan_Brightness;
        gpioPWM(get_Cyan_PWM_Pin(), Cyan_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
}


void PWM_Control::set_Red_Brightness(int _Red_Brightness) {
    if (_Red_Brightness >= 0 && _Red_Brightness <= 100) { //100 different values for duty cycle control
        Red_Brightness = _Red_Brightness;
        gpioPWM(get_Red_PWM_Pin(), Red_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
}


void PWM_Control::set_Green_Brightness(int _Green_Brightness) {
    if (_Green_Brightness >= 0 && _Green_Brightness <= 100) { //100 different values for duty cycle control
        Green_Brightness = _Green_Brightness;
        gpioPWM(get_Green_PWM_Pin(), Green_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
}


void PWM_Control::set_Blue_Brightness(int _Blue_Brightness) {
    if (_Blue_Brightness >= 0 && _Blue_Brightness <= 100) { //100 different values for duty cycle control
        Blue_Brightness = _Blue_Brightness;
        gpioPWM(get_Blue_PWM_Pin(), Blue_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
}


void PWM_Control::set_Cyan_Brightness(int _Cyan_Brightness) {
    if (_Cyan_Brightness >= 0 && _Cyan_Brightness <= 100) { //100 different values for duty cycle control
        Cyan_Brightness = _Cyan_Brightness;
        gpioPWM(get_Cyan_PWM_Pin(), Cyan_Brightness);
    } else {
        cerr << "Error: Invalid Brightness value, value range is 0-1000" << endl;
    }
}


void PWM_Control::set_Red_PWM_Pin(int _Red_PWM_Pin) {
    if (_Red_PWM_Pin >= 2 && _Red_PWM_Pin <= 27) { //starts at 2 as GPIO 0 and 1 are reserved
        Red_PWM_Pin = _Red_PWM_Pin;
    } else {
        cerr << "Error: Invalid GPIO Pin Number, the range is 0-27" << endl;
    }
}


void PWM_Control::set_Green_PWM_Pin(int _Green_PWM_Pin) {
    if (_Green_PWM_Pin >= 2 && _Green_PWM_Pin <= 27) { //starts at 2 as GPIO 0 and 1 are reserved
        Green_PWM_Pin = _Green_PWM_Pin;
    } else {
        cerr << "Error: Invalid GPIO Pin Number, the range is 0-27" << endl;
    }
}


void PWM_Control::set_Blue_PWM_Pin(int _Blue_PWM_Pin) {
    if (_Blue_PWM_Pin >= 2 && _Blue_PWM_Pin <= 27) { //starts at 2 as GPIO 0 and 1 are reserved
        Blue_PWM_Pin = _Blue_PWM_Pin;
    } else {
        cerr << "Error: Invalid GPIO Pin Number, the range is 0-27" << endl;
    }
}


void PWM_Control::set_Cyan_PWM_Pin(int _Cyan_PWM_Pin) {
    if (_Cyan_PWM_Pin >= 2 && _Cyan_PWM_Pin <= 27) { //starts at 2 as GPIO 0 and 1 are reserved
        Cyan_PWM_Pin = _Cyan_PWM_Pin;
    } else {
        cerr << "Error: Invalid GPIO Pin Number, the range is 0-27" << endl;
    }
}


void PWM_Control::set_PWM_Frequency(int _PWM_Frequency) {
    if (_PWM_Frequency >= 0 && _PWM_Frequency <= 100000) { //Max allowed PWM Frequency = 100KHz
        PWM_Frequency = _PWM_Frequency;
        gpioSetPWMfrequency(get_Red_PWM_Pin(), PWM_Frequency);
        gpioSetPWMfrequency(get_Green_PWM_Pin(), PWM_Frequency);
        gpioSetPWMfrequency(get_Blue_PWM_Pin(), PWM_Frequency);
        gpioSetPWMfrequency(get_Cyan_PWM_Pin(), PWM_Frequency);
    } else {
        cerr << "Error: Invalid PWM Frequency, the range is 0-100KHz" << endl;
    }
}


//---------------PWM Initialization Definition---------------
void PWM_Control::PWM_initialisation() {

//----------Set All PWM Pins as Outputs----------
    gpioSetMode(get_Red_PWM_Pin(), PI_OUTPUT);
    gpioSetMode(get_Green_PWM_Pin(), PI_OUTPUT);
    gpioSetMode(get_Blue_PWM_Pin(), PI_OUTPUT);
    gpioSetMode(get_Cyan_PWM_Pin(), PI_OUTPUT);
//-----------------------------------------------


//----------Set All PWM Ranges for PWM Pins----------    
    gpioSetPWMrange(get_Red_PWM_Pin(), 100);
    gpioSetPWMrange(get_Green_PWM_Pin(), 100);
    gpioSetPWMrange(get_Blue_PWM_Pin(), 100);
    gpioSetPWMrange(get_Cyan_PWM_Pin(), 100);
}
//---------------------------------------------------

//-----------------------------------------------------------


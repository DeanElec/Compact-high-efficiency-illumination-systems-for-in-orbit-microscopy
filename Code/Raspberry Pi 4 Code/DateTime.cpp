#include "DateTime.h"
#include <iomanip>
#include <sstream>
#include <ctime>

using std::string;

DateTime& DateTime::getInstance() {
    static DateTime instance;
    return instance;
}

string DateTime::getDateTime() const {
    //get current time as a time_t object
    auto t = std::time(nullptr);
    //convert to tm struct
    auto tm = *std::localtime(&t);

    //use ostringstream to format the date and time as a string
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H:%M:%S");
    return oss.str();
}


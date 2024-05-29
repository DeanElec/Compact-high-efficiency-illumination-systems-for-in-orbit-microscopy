#ifndef DATETIME_H
#define DATETIME_H

#include <string>

class DateTime {
public:
    static DateTime& getInstance();
    std::string getDateTime() const;

private:
    DateTime() {} 
    DateTime(const DateTime&) = delete; 
    DateTime& operator=(const DateTime&) = delete; 
};

#endif


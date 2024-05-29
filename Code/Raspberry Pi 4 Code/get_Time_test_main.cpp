#include <iostream>
#include "DateTime.h"
#include <string.h>

int main() {
    std::string test = DateTime::getInstance().getDateTime();
    // Output the date and time string
    std::cout << "Current date and time as string: " << test << std::endl;



    return 0;
}


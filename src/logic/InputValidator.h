#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <string>

class InputValidator {
public:
    static bool isValidUsername(const std::string& username);
    static bool isValidMessage(const std::string& message);
};

#endif

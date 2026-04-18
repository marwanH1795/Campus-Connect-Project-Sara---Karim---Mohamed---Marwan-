#include "InputValidator.h"

bool InputValidator::isValidUsername(const std::string& username) {
    return !username.empty() && username.length() <= 20;
}

bool InputValidator::isValidMessage(const std::string& message) {
    return !message.empty();
}

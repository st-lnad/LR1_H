#pragma once

#include <iostream>
#include <charconv>
#include <string>

bool inline TryParseInt(const std::string& s, int& out)
{
    auto result = std::from_chars(s.data(), s.data() + s.size(), out);
    return result.ec == std::errc() && result.ptr == s.data() + s.size();
}

void inline CinIntValue(int& value, const char* InviteMessage, const char* ErrorMessage) {
    bool success = false;
    do {
        std::cout << InviteMessage;
        std::cin >> value;
        success = !std::cin.fail();
        if (success == false) {
            std::cin.clear();
            std::cout << ErrorMessage << std::endl;
        }
    } while (success == false);
}

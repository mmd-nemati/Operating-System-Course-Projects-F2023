#include "../lib/utils.hpp"

std::string extract_building_name(std::string path) {
    std::stringstream stream(path);
    std::string token;
    int i = 0;
    while (getline(stream, token, '/')) {
        if (i == 1)
            return token;
        i++;
    }

    return std::string();
}

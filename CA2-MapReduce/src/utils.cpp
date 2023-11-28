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

void log(const char* msg) {
    int fd = open("log.txt", O_APPEND | O_CREAT | O_RDWR, 0777);
    write(fd,"-- ", 3);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
    close(fd);
}
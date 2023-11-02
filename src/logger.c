#include "../lib/logger.h"
#include "../lib/colors.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void logTerminalNormal(const char* msg) {
    write(1, msg, strlen(msg));
    write(1, "\n", 1);
}

void logTerminalInput(const char* msg) {
    write(1, ANSI_GRN "[Input] " ANSI_RST, 8 + ANSI_LEN);
    write(1, msg, strlen(msg));
}

void logTerminalMsg(const char* msg) {
    write(1, ANSI_WHT "[Message] " ANSI_RST, 10 + ANSI_LEN);
    write(1, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
}

void logTerminalInfo(const char* msg) {
    write(1, ANSI_BLU "[Info] " ANSI_RST, 7 + ANSI_LEN);
    write(1, msg, strlen(msg));
    write(1, "\n", 1);
}

void logTerminalWarning(const char* msg) {
    write(1, ANSI_YEL "[Warning] " ANSI_RST, 10 + ANSI_LEN);
    write(1, msg, strlen(msg));
    write(1, "\n", 1);
}

void logTerminalError(const char* msg) {
    write(2, ANSI_RED "[Error] " ANSI_RST, 8 + ANSI_LEN);
    write(2, msg, strlen(msg));
    write(2, "\n", 1);
}
// void logNormal(const char* msg) {
//     int fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR);
//     write(fd, msg, strlen(msg));
//     write(fd, "\n", 1);
//     close(fd);
// }

// void logInput(const char* msg) {
//     int fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR);
//     write(fd, ANSI_GRN "[Input] " ANSI_RST, 8 + ANSI_LEN);
//     write(fd, msg, strlen(msg));
//     close(fd);
// }

void logFile(const char* msg, const char* username) {
    int fd = open("log.txt", O_APPEND | O_CREAT | O_RDWR, 0777);
    write(fd,"[User] ", 7);
    write(fd, username, strlen(username));
    write(fd," --> ", 5);
    write(fd, msg, strlen(msg));
    write(fd, "\n", 1);
    close(fd);
}

// void logInfo(const char* msg) {
//     int fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR);
//     write(fd, ANSI_BLU "[Info] " ANSI_RST, 7 + ANSI_LEN);
//     write(fd, msg, strlen(msg));
//     write(fd, "\n", 1);
//     close(fd);
// }

// void logWarning(const char* msg) {
//     int fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR);
//     write(fd, ANSI_YEL "[Warning] " ANSI_RST, 10 + ANSI_LEN);
//     write(fd, msg, strlen(msg));
//     write(fd, "\n", 1);
//     close(fd);
// }

// void logError(const char* msg) {
//     int fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR);
//     write(fd, ANSI_RED "[Error] " ANSI_RST, 8 + ANSI_LEN);
//     write(fd, msg, strlen(msg));
//     write(fd, "\n", 1);
//     close(fd);
// }
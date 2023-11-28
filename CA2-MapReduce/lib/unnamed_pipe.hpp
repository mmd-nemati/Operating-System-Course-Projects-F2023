#ifndef __UNNAMED_PIPE_HPP__
#define __UNNAMED_PIPE_HPP__

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#include "types.hpp"

std::vector<int> create_pipe();
bool close_fd(int fd);
int write_fd(const char *buffer, int size, int write_fd);
std::string read_fd(int read_fd);

#endif
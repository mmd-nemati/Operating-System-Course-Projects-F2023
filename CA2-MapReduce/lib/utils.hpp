#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

std::string extract_building_name(std::string path);
void log(const char* msg);

#endif
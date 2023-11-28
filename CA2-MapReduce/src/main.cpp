
#include "../lib/main_proc.hpp"

int main(int argc, char* argv[]) {
    int fd = open("log.txt", O_CREAT | O_TRUNC, 0777);
    close(fd);
    log(std::string("-----------\nNew program initiated\n-----------").c_str());

    if (argc < 2) 
        throw std::runtime_error("Buildings directory not specified.");
    if (getuid() != 0) {
        std::cerr << "Please run the program with sudo permissions.\nIt has no harm :)\nWe just want to increase the size of the pipe." << std::endl;
        return 1;
    }
    
    MainProc* mp = new MainProc(argv[1]);
    mp->run();
    log(std::string("Main process finished").c_str());
    return 0;
}
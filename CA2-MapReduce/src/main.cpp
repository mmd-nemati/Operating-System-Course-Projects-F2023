
#include "../lib/main_proc.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) 
        throw std::runtime_error("Buildings directory not specified.");
    MainProc* mp = new MainProc(argv[1]);
    mp->run();
    
    return 0;
}
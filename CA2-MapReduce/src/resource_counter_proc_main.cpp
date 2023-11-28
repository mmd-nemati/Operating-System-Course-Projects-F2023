#include "../lib/resource_counter_proc.hpp"

int main(int argc, char const *argv[]) {
    if (argc != 3)
        exit(EXIT_FAILURE);
    ResourceCounterProc* rcp = new ResourceCounterProc(argv[1], std::stoi(argv[2]));
    rcp->run();
    exit(0);
    return 0;
}

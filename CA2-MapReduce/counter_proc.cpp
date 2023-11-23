#include <iostream>
#include <unistd.h>

#include "resource_counter.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>\n";
        return 1;
    }

    ResourceCounter* counter = new ResourceCounter(argv[1]); 
    std::vector<Record*> records = counter->read_records();

    // encodings
    // pipe constructions


    return 0;
}
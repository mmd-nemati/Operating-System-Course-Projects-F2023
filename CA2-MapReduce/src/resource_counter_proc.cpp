#include "../lib/resource_counter_proc.hpp"
#include <iostream>
ResourceCounterProc::ResourceCounterProc(std::string _file_path, int _pipe_fd) {
    file_path = _file_path;
    pipe_fd = _pipe_fd;
    counter = new ResourceCounter(_file_path.c_str());
}

ResourceCounterProc::~ResourceCounterProc() {
    close_fd(pipe_fd);
}

void ResourceCounterProc::run() {
    std::string encoded_records = RecordSerializer::encode(counter->read_records());
    // std::cout << "WROTE: pipe" << encoded_records.size() << std::endl;
    int s = write_fd(encoded_records.c_str(), encoded_records.size(), pipe_fd);
    // std::cout << "WROTE: s" << s << std::endl;
}

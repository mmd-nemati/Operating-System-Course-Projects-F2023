#include "../lib/resource_counter_proc.hpp"

ResourceCounterProc::ResourceCounterProc(std::string _file_path, int _pipe_fd) {
    file_path = _file_path;
    pipe_fd = _pipe_fd;
    counter = new ResourceCounter(_file_path.c_str());
    log(std::string("ResourceCounter process in path " + file_path + " started").c_str());
}

ResourceCounterProc::~ResourceCounterProc() {
    close_fd(pipe_fd);
    log(std::string("ResourceCounter process in path " + file_path + " finished").c_str());
}

void ResourceCounterProc::run() {
    std::string encoded_records = RecordSerializer::encode(counter->read_records());
    int s = write_fd(encoded_records.c_str(), encoded_records.size(), pipe_fd);
    log(std::string("ResourceCounter process in path " + file_path + " sent data to Building process").c_str());
}

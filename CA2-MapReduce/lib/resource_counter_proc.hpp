#ifndef __RESOURCE_COUNTER_PROC_HPP__
#define __RESOURCE_COUNTER_PROC_HPP__

#include "unnamed_pipe.hpp"
#include "resource_counter.hpp"
#include "record_serializer.hpp"
#include "utils.hpp"


class ResourceCounterProc {
    public:
        ResourceCounterProc(std::string _file_path, int _pipe_fd);
        ~ResourceCounterProc();
        ResourceCounter* counter;
        std::map<std::string, ResourceType> resource_type_map;
        void run();
        
    private:
        int pipe_fd;
        std::string file_path;
};

#endif
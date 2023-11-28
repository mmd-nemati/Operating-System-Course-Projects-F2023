#ifndef __BUILDING_PROC_HPP__
#define __BUILDING_PROC_HPP__

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <sstream>
#include <memory>
#include <regex>

#include "building.hpp"
#include "named_pipe.hpp"
#include "unnamed_pipe.hpp"
#include "utils.hpp"
#include "types.hpp"

class BuildingProc {
    public:
        BuildingProc(std::string _path, int _write_pipe_fd, int _read_pipe_fd, 
            std::vector<std::vector<int>> *_resources_read_pipes);
        void run();
        ~BuildingProc();

    private:
        Building* building;
        void save_records();
        std::string read_cmd_pipe();
        BuildingRequestData* decode_cmd(std::string cmd);
        std::map<std::string, ResourceType> resource_type_map;
        int write_pipe_fd;
        int read_pipe_fd;
        std::string path;
        std::string name;
        std::shared_ptr<NamedPipeServer> building_server;
        std::shared_ptr<NamedPipeClient> building_client;
        std::vector<std::vector<int>> *resources_read_pipes;
        std::map<std::string, std::vector<std::map<std::string, int>>> resource_records_map;
        std::vector<std::map<std::string, double>> bills_map;
};

#endif
#ifndef __MAIN_PROC_HPP__
#define __MAIN_PROC_HPP__


#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include "unnamed_pipe.hpp"
#include "types.hpp"
#include "colors.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

class MainProc {
    public:
        MainProc(std::string _buildings_dir);
        void run();
        ~MainProc();

    private:
        void ready();
        std::map<std::string, int> building_names_map;
        std::string buildings_dir;
        std::vector<std::vector<int>> main_read_pipes;
        std::vector<std::vector<int>> main_write_pipes;
        void make_buildings_map();
        void make_buildings();
        void make_bills_center();
        void get_req(std::string &req_buildings, std::string &req_resources,
        std::string &req_reports, std::string &req_month);

        void send_req(std::string &req_buildings, std::string &req_resources,
        std::string &req_reports, std::string &req_month);
        void recieve_response();

};

#endif
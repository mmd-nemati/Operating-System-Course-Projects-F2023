#ifndef __MAIN_PROC_HPP__
#define __MAIN_PROC_HPP__


#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include "unnamed_pipe.hpp"
#include "types.hpp"

namespace fs = std::filesystem;

class MainProc {
    public:
        MainProc(std::string _buildings_dir_path);
        void run();
        ~MainProc();

    private:
        std::map<std::string, int> building_names_map;
        std::string buildings_dir_path;
        std::vector<std::vector<int>> main_read_pipes;
        std::vector<std::vector<int>> main_write_pipes;
        void create_building_names_map();
        void make_buildings();
        void make_bills_center();
        void get_user_request(std::string &buidings_requested, std::string &resources_requested,
                std::string &report_parameter_requested, std::string &month_requestd);

        void send_user_request_to_buildings(std::string &buidings_requested,std::string &resources_requested,
            std::string &report_parameter_requested, std::string &month_requestd);
        void recieve_reports();

};

#endif
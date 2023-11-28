#include "../lib/main_proc.hpp"

MainProc::MainProc(std::string _buildings_dir_path) {
    buildings_dir_path = _buildings_dir_path;
}

MainProc::~MainProc() {
    for (int i = 0; i < int(main_read_pipes.size()); i++) {
        close_fd(main_read_pipes[i][0]);
        close_fd(main_write_pipes[i][1]);
    }
}

void MainProc::create_building_names_map() {
    int folder_count = 0;

    for (auto &entry : fs::directory_iterator(buildings_dir_path)){
        if (fs::is_directory(entry)) {
            main_read_pipes.push_back(create_pipe());
            main_write_pipes.push_back(create_pipe());
            std::string building_name = entry.path().filename().string();
            building_names_map[building_name] = folder_count;
            folder_count++;
        }
    }
}

void MainProc::make_buildings() {
    std::map<std::string, int>::iterator it = building_names_map.begin();
    while (it != building_names_map.end()) {
        int pid = fork();
        if (pid == -1)
            throw std::runtime_error("Fork failed building");
        
        else if (pid == 0) {
            close_fd(main_read_pipes[it->second][0]);
            close_fd(main_write_pipes[it->second][1]);

            execl(PATH_TO_BULDING_PROGRAM.c_str(), PATH_TO_BULDING_PROGRAM.c_str(), 
            (buildings_dir_path + "/" + it->first).c_str(),
                  std::to_string(main_read_pipes[it->second][1]).c_str(),
                std::to_string(main_write_pipes[it->second][0]).c_str(), nullptr);

            return;
        }
        else {
            close_fd(main_read_pipes[it->second][1]);
            close_fd(main_write_pipes[it->second][0]);
        }
        ++it;
    }
}

void MainProc::make_bills_center() {
    int pid = fork();
    if (pid == -1)
        throw std::runtime_error("Fork failed bills");

    else if (pid == 0) {
        execl(PATH_TO_BILLS_PROGRAM.c_str(), PATH_TO_BILLS_PROGRAM.c_str(),
         std::to_string(main_read_pipes.size()).c_str(), nullptr);
        return;
    }
}

void MainProc::get_user_request(std::string &buidings_requested, std::string &resources_requested,
        std::string &report_parameter_requested, std::string &month_requestd) {
    std::cout << GET_BUILDINGS_PROMPT;
    std::getline(std::cin, buidings_requested);
    std::cout << GET_RESOURCES_PROMPT;
    std::getline(std::cin, resources_requested);
    std::cout << GET_REPORT_PROMPT;
    std::getline(std::cin, report_parameter_requested);
    std::cout << GET_MONTH_PROMPT;
    std::getline(std::cin, month_requestd);
}

void MainProc::send_user_request_to_buildings(std::string &buidings_requested,std::string &resources_requested,
    std::string &report_parameter_requested, std::string &month_requestd) {
    std::map<std::string, int>::iterator it = building_names_map.begin();

    while (it != building_names_map.end()) {
        std::stringstream stream(buidings_requested);
        std::string building;
        bool found = false;
        while (std::getline(stream, building, ' ')) {
            if (it->first == building) {
                
                found = true;
                std::string msg = month_requestd + '\n' + resources_requested + '\n' + report_parameter_requested;
                int s = write_fd(msg.c_str(), msg.size(), main_write_pipes[it->second][1]);
                // std::cout << "Building : " << s << std::endl;
            }
        }
        if (!found)
            write_fd("kill", strlen("kill"), main_write_pipes[it->second][1]);
        //     throw std::runtime_error("Building not found.");

        ++it;
    }
}

void MainProc::recieve_reports()
{
    // map<string, int>::iterator it = building_names_map.begin();
    // while (it != building_names_map.end())
    // {
    //     string data = read_fd(main_read_pipes[it->second][0]);
    //     if (data != "")
    //     {
    //         cout << "== Building: " << it->first << " ==" << endl;
    //         cout << data << endl;
    //     }
    //     ++it;
    // }
    return;
}

void MainProc::run() {
    create_building_names_map();
    this->make_bills_center();
    this->make_buildings();
    std::string buidings_requested, resources_requested, reports_requested, month_requested;
    get_user_request(buidings_requested, resources_requested, reports_requested, month_requested);


    send_user_request_to_buildings(buidings_requested, resources_requested, reports_requested, month_requested);
    for (int i = 0; i < int(main_read_pipes.size()) + 1; i++)
        wait(NULL);
    std::cout << "--------------------------------asas" << std::endl;
    // return;
    recieve_reports();
}
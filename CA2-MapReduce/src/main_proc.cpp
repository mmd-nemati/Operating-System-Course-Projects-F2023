#include "../lib/main_proc.hpp"

MainProc::MainProc(std::string _buildings_dir) {
    buildings_dir = _buildings_dir;
    log(std::string("Main process started").c_str());
}

MainProc::~MainProc() {
    for (int i = 0; i < int(main_read_pipes.size()); i++) {
        close_fd(main_read_pipes[i][0]);
        close_fd(main_write_pipes[i][1]);
        log(std::string("Main process closed pipes " + std::to_string(main_read_pipes[i][0]) + " and " +
            std::to_string(main_write_pipes[i][1])).c_str());
    }
    log(std::string("Main process finished").c_str());
}

void MainProc::make_buildings_map() {
    int folder_count = 0;

    for (auto &entry : fs::directory_iterator(buildings_dir)){
        if (fs::is_directory(entry)) {
            main_read_pipes.push_back(create_pipe());
            main_write_pipes.push_back(create_pipe());
            std::string building_name = entry.path().filename().string();
            building_names_map[building_name] = folder_count;
            folder_count++;
        }
    }
}

void MainProc::show_buildings() {
    std::map<std::string, int>::iterator it = building_names_map.begin();
    std::cout << ANSI_YEL << PROMPTS_DELIMITER <<  ANSI_RED << "==== list of " << ANSI_BLU <<
     building_names_map.size() << ANSI_RED << " buildings ====" << std::endl;
    while (it != building_names_map.end()) {
        std::cout << ANSI_BLU << "\t    " << it->first << ANSI_RST << std::endl;
        ++it; 
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
            (buildings_dir + "/" + it->first).c_str(),
                  std::to_string(main_read_pipes[it->second][1]).c_str(),
                std::to_string(main_write_pipes[it->second][0]).c_str(), nullptr);

            return;
        }
        else {
            close_fd(main_read_pipes[it->second][1]);
            close_fd(main_write_pipes[it->second][0]);
        }
        log(std::string("Main process made Building process " + it->first).c_str());
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
    log(std::string("Main process made Bills process").c_str());
}


void MainProc::get_req(std::string &req_buildings, std::string &req_resources,
        std::string &req_reports, std::string &req_month) {
    std::cout << ANSI_YEL << PROMPTS_DELIMITER << ANSI_GRN << GET_BUILDINGS_PROMPT << ANSI_RST;
    std::getline(std::cin, req_buildings);

    std::cout << ANSI_YEL << PROMPTS_DELIMITER << ANSI_GRN << GET_RESOURCES_PROMPT << ANSI_RST;
    std::getline(std::cin, req_resources);
    
    std::cout << ANSI_YEL << PROMPTS_DELIMITER << ANSI_GRN << GET_REPORT_PROMPT << ANSI_RST;
    std::getline(std::cin, req_reports);

    std::cout << ANSI_YEL << PROMPTS_DELIMITER << ANSI_GRN << GET_MONTH_PROMPT << ANSI_RST;
    std::getline(std::cin, req_month);
    std::cout << ANSI_YEL << PROMPTS_DELIMITER << ANSI_RST;
    log(std::string("Main process recieved inputs").c_str());
}

void MainProc::send_req(std::string &req_buildings,std::string &req_resources,
    std::string &req_reports, std::string &req_month) {
    std::map<std::string, int>::iterator it = building_names_map.begin();
    bool found;
    while (it != building_names_map.end()) {
        std::stringstream stream(req_buildings);
        std::string building;
        found = false;
        while (std::getline(stream, building, ' ')) {
            if (it->first == building) {
                found = true;
                std::string msg = req_month + '\n' + req_resources + '\n' + req_reports;
                int s = write_fd(msg.c_str(), msg.size(), main_write_pipes[it->second][1]);
                log(std::string("Main process sent request to Building process " + it->first).c_str());

            }
        }
        if (!found) {
            write_fd(KILL_CMD.c_str(), KILL_CMD.size(), main_write_pipes[it->second][1]);
            log(std::string("Main process killed Building process " + it->first).c_str());
        }
        ++it;
    }
}

void MainProc::recieve_response() {
    std::map<std::string, int>::iterator it = building_names_map.begin();
    while (it != building_names_map.end()) {
        std::string data = read_fd(main_read_pipes[it->second][0]);
        if (data != "") {
            std::cout << ANSI_RED << "==== building: " << ANSI_BLU << it->first << ANSI_RED << " ====" <<  ANSI_RST << std::endl;
            std::cout << data << std::endl;
            log(std::string("Main process recieved response from Building process").c_str());
            return;
        }
        ++it;
    }
}

void MainProc::ready() {
    this->make_buildings_map();
    this->show_buildings();
    this->make_bills_center();
    this->make_buildings();
}

void MainProc::run() {
    this->ready();
    std::string req_buildings, req_resources, reports_requested, month_requested;

    get_req(req_buildings, req_resources, reports_requested, month_requested);
    send_req(req_buildings, req_resources, reports_requested, month_requested);
    for (int i = 0; i < int(main_read_pipes.size()) + 1; i++)
        wait(NULL);

    recieve_response();
}
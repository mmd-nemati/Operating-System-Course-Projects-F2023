#include "../lib/building_proc.hpp"
#include <cstring>
BuildingProc::BuildingProc(std::string _path, int _write_pipe_fd, int _read_pipe_fd, 
        std::vector<std::vector<int>> *_resources_read_pipes) {
    path = _path;
    name = extract_building_name(path);
    write_pipe_fd = _write_pipe_fd;
    read_pipe_fd = _read_pipe_fd;
    resources_read_pipes = _resources_read_pipes;
    building = new Building(10, name);
    resource_type_map = {
        {"gas", ResourceType::GAS},
        {"water", ResourceType::WATER},
        {"electricity", ResourceType::ELEC}
    };
    building_server = std::make_shared<NamedPipeServer>(name);
    building_client = std::make_shared<NamedPipeClient>(BILLS_SERVER);
    log(std::string("Building process " + name + " started").c_str());
    // std::cout << "Building process1111" << std::endl;

}

BuildingProc::~BuildingProc() {
        for (int i = 0; i < int(resources_read_pipes->size()); i++)
    {
        close_fd((*resources_read_pipes)[i][0]);
    }

    close_fd(write_pipe_fd);
    close_fd(read_pipe_fd);
    NamedPipe::remove_pipe(name);
    NamedPipe::remove_pipe(BILLS_SERVER);
    log(std::string("Building process " + name + " finished").c_str());
}

void BuildingProc::save_records() {
    std::string data = read_fd((*resources_read_pipes)[0][0]);
    // std::cout << "READ: 2" << data << std::endl;
    building->save_records(data.c_str(), ResourceType::GAS);
    data = read_fd((*resources_read_pipes)[1][0]);
    building->save_records(data.c_str(), ResourceType::WATER);
    data = read_fd((*resources_read_pipes)[2][0]);
    building->save_records(data.c_str(), ResourceType::ELEC);
    log(std::string("Building process " + name + " saved all records").c_str());
}

std::string BuildingProc::read_cmd_pipe() {
    std::string cmd;
    while (true) {
        cmd = read_fd(read_pipe_fd);
        if (cmd != "")
            break;
    }

    return cmd;
}

BuildingRequestData* BuildingProc::decode_cmd(std::string cmd) {
    BuildingRequestData* data = new BuildingRequestData();
    std::istringstream iss(cmd);
    std::string line;
    data->report = KILL_CMD;
    std::getline(iss, line);
    if (line == KILL_CMD)
        return data;
    data->month = std::stoi(line);

    std::getline(iss, line);
    std::istringstream resource_stream(line);
    std::string resource;
    while (std::getline(resource_stream, resource, '/')) {
        data->resources.push_back(resource);
    }

    std::getline(iss, data->report);

    return data;
}

void BuildingProc::run() {
    std::regex e ("(\\d+\\.\\d{2})"); 
    std::smatch match;
    std::string cmd, cost, rec_data;
    this->save_records();
    cmd = this->read_cmd_pipe();
    BuildingRequestData* data = this->decode_cmd(cmd);
    if (data->report == KILL_CMD)
        return;
    
    
    cost.append("== Report: " + data->report  + " ==\n");
    for (int i = 0; i < data->resources.size(); i++) {
        std::string encoded_records = building->get_records(resource_type_map[data->resources[i]]);
        if (data->report == BILLS_REPORT) {
            building_client->send(REQUEST_BILLS_PREFIX + name + '\n' + 
                std::to_string(data->month) + '\n' + data->resources[i] + '\n' + encoded_records);
           
            while (true) {
                rec_data = building_server->receive();
                if (rec_data.compare(0, 4, "cost") == 0) {
                    std::regex_search(rec_data, match, e);
                    cost.append(data->resources[i] + " --> " + match.str() + "\n");
                    break;
                }    
            }
        }
        else if (data->report == WHOLE_USAGE_REPORT) {
            rec_data = std::to_string(building->calculate_monthly_usage(resource_type_map[data->resources[i]], data->month));
            cost.append(data->resources[i] + " --> " + rec_data + "\n");
        }
        else if (data->report == MAX_USAGE_HOUR_REPORT) {
            rec_data = std::to_string(building->calculate_max_usage_hour(resource_type_map[data->resources[i]], data->month));
            cost.append(data->resources[i] + " --> " + rec_data + "\n");
        }
        else if (data->report == DIFF_MAX_AVG_REPORT) {
            rec_data = std::to_string(building->calculate_diff_max_avg(resource_type_map[data->resources[i]], data->month));
            cost.append(data->resources[i] + " --> " + rec_data + "\n");
        }
        else if (data->report == AVERAGE_USAGE_REPORT) {
            rec_data = std::to_string(building->calculate_avg_usage(resource_type_map[data->resources[i]], data->month));
            cost.append(data->resources[i] + " --> " + rec_data + "\n");
        }
        log(std::string("Building process " + name + " made output: " + cost).c_str());
    }
    write_fd(cost.c_str(), cost.size(), write_pipe_fd);
    log(std::string("Building process " + name + " sent output to main").c_str());
    building_client->send(KILL_CMD);
    log(std::string("Building process " + name + " killed Bills process").c_str());
}
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
    // std::cout << "Building process1111" << std::endl;

}

BuildingProc::~BuildingProc() {
    NamedPipe::remove_pipe(BILLS_SERVER);
}

void BuildingProc::save_records() {
    std::string data = read_fd((*resources_read_pipes)[0][0]);
    // std::cout << "READ: 2" << data << std::endl;
    building->save_records(data.c_str(), ResourceType::GAS);
    data = read_fd((*resources_read_pipes)[1][0]);
    building->save_records(data.c_str(), ResourceType::WATER);
    data = read_fd((*resources_read_pipes)[2][0]);
    building->save_records(data.c_str(), ResourceType::ELEC);
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
    data->report = "kill";
    std::getline(iss, line);
    if (line == "kill")
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
    this->save_records();
    std::string cmd = this->read_cmd_pipe();
    BuildingRequestData* data = this->decode_cmd(cmd);
    if (data->report == "kill")
        return;
    // std::cout << "aaa->>>>" << data->report << std::endl;
    for (int i = 0; i < data->resources.size(); i++) {
        std::string encoded_records = building->get_records(resource_type_map[data->resources[i]]);
        // std::cout << encoded_records << std::endl;
        if (data->report == "bills") {
            building_client->send(REQUEST_BILLS_PREFIX + name + '\n' + 
            std::to_string(data->month) + '\n' + data->resources[i] + '\n' + encoded_records);
           std::string cost;
            while(true) {
                cost = building_server->receive();
                if (cost.compare(0, 4, "cost") == 0) {
                    std::cout << "cost: " << cost << std::endl;
                    break;
                }    
            }
        }

    }
    // if (cmd == KILL_PROCESS_MSG)
    //     return;
    // string report_parameter;
    // vector<string> resources;
    // main_cmd_decoder(cmd, resources, report_parameter);
    // string building_record = create_record_building(report_parameter, resources);
    // write_fd(building_record.c_str(), building_record.size(), write_pipe_fd);
}
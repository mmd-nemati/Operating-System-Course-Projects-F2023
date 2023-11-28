#include "../lib/building_proc.hpp"

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        exit(EXIT_FAILURE);
    }

    std::string building_dir_path = argv[1];
    std::vector<std::vector<int>> resources_pipes;
    std::string resource_types[] = {"Water", "Electricity", "Gas"};

    for (int i = 0; i < 3; i++) {
        resources_pipes.push_back(create_pipe());
        int pid = fork();
        if (pid == -1)
            throw std::runtime_error("Fork failed");
        
        else if (!pid) {
            close_fd(resources_pipes[i][0]);
            int fil = execl(PATH_TO_RESOURCE_PROGRAM.c_str(), PATH_TO_RESOURCE_PROGRAM.c_str(),
                 (building_dir_path + "/" + resource_types[i] + ".csv").c_str(), 
                 std::to_string(resources_pipes[i][1]).c_str(), nullptr);
            return 1;
        }
        else
            close_fd(resources_pipes[i][1]);
    }

    for (int i = 0; i < 3; i++)
        wait(NULL);
    // std::cout << "REACHED BUILDING PROCESS:" << std::endl;
    
    BuildingProc* bp = new BuildingProc(building_dir_path, atoi(argv[2]), 
        atoi(argv[3]), &resources_pipes);

    bp->run();
    std::cout << "building finished " << std::endl;
    exit(0);
    return 0;
}

#ifndef __BILLS_PROC_HPP__
#define __BILLS_PROC_HPP__

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

#include "bills.hpp"
#include "named_pipe.hpp"
#include "utils.hpp"

class BillsProc {
    public:
        BillsProc(int _building_count);
        void run();
        ~BillsProc();
    private:
        int building_count;
        Bills* bills;
        std::shared_ptr<NamedPipeServer> bills_server;
        std::shared_ptr<NamedPipeClient> bills_client;
};

#endif
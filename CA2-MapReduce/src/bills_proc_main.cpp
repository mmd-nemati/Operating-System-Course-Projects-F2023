#include "../lib/bills_proc.hpp"

int main(int argc, char const *argv[]) {
    BillsProc* bp = new BillsProc(std::stoi(argv[1]));
    bp->run();
    exit(0);
    return 0;

}

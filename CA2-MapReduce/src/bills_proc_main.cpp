#include "../lib/bills_proc.hpp"

int main(int argc, char const *argv[]) {
    BillsProc* bp = new BillsProc(std::stoi(argv[1]));
    bp->run();
    std::cout << "BILLS FINISHEDDDDD" << std::endl;
    exit(0);
    return 0;

}

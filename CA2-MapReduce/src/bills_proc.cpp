#include "../lib/bills_proc.hpp"

BillsProc::BillsProc(int _building_count) {
    building_count = _building_count;
    bills = new Bills(10, BILLS_COEFF_FILE_PATH);
    bills_server = std::make_shared<NamedPipeServer>(BILLS_SERVER);
    // std::cout << "BILLSS COMPLETED" << std::endl;
}

BillsProc::~BillsProc() {
    NamedPipe::remove_pipe(BILLS_SERVER);
}

void BillsProc::run() {
    // std::string prefix = REQUEST_BILLS_PREFIX;
    bills->read_coeffs();
    int msg_recieved_count = 0;
    while (true) {
        std::string rec_data = bills_server->receive();
    /*
      name
      month
      gas
      data ha
    */
    if (rec_data.compare(0, REQUEST_BILLS_PREFIX.size(), REQUEST_BILLS_PREFIX) == 0) {
    // std::cout << "--------------Received billd: " << rec_data << std::endl;
        rec_data = rec_data.substr(REQUEST_BILLS_PREFIX.size());
        RequestBillsData* data = bills->decode_request(rec_data);
        // std::cout << data->records << std::endl;
        bills->save_records(data->records.c_str());
        std::string cost;
        cost.append("cost:");
        cost.append(std::to_string(bills->calculate_bill(data->type, data->month)));

        bills_client = std::make_shared<NamedPipeClient>(data->server_name);
        bills_client->send(cost);
        break;
    }
    }
}
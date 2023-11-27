#include "../lib/bills_proc.hpp"

BillsProc::BillsProc(int _building_count) {
    building_count = _building_count;
    bills_server = std::make_shared<NamedPipeServer>(BILLS_SERVER);
    bills = new Bills(10, BILLS_COEFF_FILE_PATH);
}

BillsProc::~BillsProc() {
    NamedPipe::remove_pipe(BILLS_SERVER);
}

void BillsProc::run() {
    std::string prefix = REQUEST_BILLS_PREFIX;
    int msg_recieved_count = 0;
    std::string rec_data = bills_server->receive();
    /*10
      gas
      dataha
    */
    if (rec_data.compare(0, prefix.size(), prefix) == 0) {
        rec_data = rec_data.substr(prefix.size());
        // decode_building_data(rec_data);
        // vector<MonthBill> bills = create_bills();
        RequestBillsData* data = bills->decode_request(rec_data);
        bills->save_records(data->records.c_str());
        std::string cost = std::to_string(bills->calculate_bill(data->type, data->month));
        bill_client->send(cost);
        // msg_recieved_count++;
    }
    // if (msg_recieved_count == buildings_count)
    //         return;
    // // }
}
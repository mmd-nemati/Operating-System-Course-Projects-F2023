#include "../lib/bills_proc.hpp"

BillsProc::BillsProc(int _building_count) {
    building_count = _building_count;
    bills = new Bills(10, BILLS_COEFF_FILE_PATH);
    bills_server = std::make_shared<NamedPipeServer>(BILLS_SERVER);
    log("Bills process started");
}

BillsProc::~BillsProc() {
    NamedPipe::remove_pipe(BILLS_SERVER);
    log("Bills process finished");
}

void BillsProc::run() {
    bills->read_coeffs();
    while (true) {
        std::string rec_data = bills_server->receive();
        if (rec_data.compare(0, KILL_CMD.size(), KILL_CMD) == 0)
            break;

        else if (rec_data.compare(0, REQUEST_BILLS_PREFIX.size(), REQUEST_BILLS_PREFIX) == 0) {
            rec_data = rec_data.substr(REQUEST_BILLS_PREFIX.size());
            RequestBillsData* data = bills->decode_request(rec_data);
            bills->save_records(data->records.c_str());
            std::string cost;
            cost.append("cost:");
            cost.append(std::to_string(bills->calculate_bill(data->type, data->month)));

            bills_client = std::make_shared<NamedPipeClient>(data->server_name);
            bills_client->send(cost);
            log(std::string("Bills sent " + cost + " to " + data->server_name).c_str());
        }
    }
}
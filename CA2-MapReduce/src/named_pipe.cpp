#include "../lib/named_pipe.hpp"

NamedPipe::NamedPipe(const std::string &pipe_name)
    : pipe_name_(pipe_name) {}

void NamedPipe::remove_pipe(const std::string &pipe_path) {
    remove(pipe_path.c_str());
}

NamedPipeClient::NamedPipeClient(const std::string &pipe_name)
    : NamedPipe(pipe_name) {
    do {
        pipe_fd_ = open(pipe_name.c_str(), O_WRONLY);
    } while (pipe_fd_ == -1);
}

void NamedPipeClient::send(const std::string &msg) {
    int sent_bytes = write(pipe_fd_, msg.c_str(), msg.size());
    if (sent_bytes != msg.size())
        throw std::runtime_error("Sending message failed.");
}

NamedPipeServer::NamedPipeServer(const std::string &pipe_name)
    : NamedPipe(pipe_name) {
    int res, fifo_fd;
    if ((pipe_fd_ = open(pipe_name.c_str(), O_RDWR | O_NONBLOCK)) == -1) {
        fifo_fd = mkfifo(pipe_name.c_str(), 0777);
        if (fifo_fd != 0)
            throw std::runtime_error("Couldn't make FIFO file.");

        else
            pipe_fd_ = open(pipe_name_.c_str(), O_RDWR | O_NONBLOCK);
    }
    res = fcntl(pipe_fd_, F_SETPIPE_SZ, 16384);
    if (res == -1)
        throw std::runtime_error("Couldn't increase pipe size.");

    if (pipe_fd_ == -1)
        throw std::runtime_error("Couldn't open server side of named pipe.(" + pipe_name + ")");
}

NamedPipeClient::~NamedPipeClient() {
    close(pipe_fd_);
}

std::string NamedPipeServer::receive() {
    char buffer[BUFFER_SIZE];
    std::string result;

    while (true) {
        int read_bytes = read(pipe_fd_, buffer, BUFFER_SIZE);
        if (read_bytes == -1) {
            break;
        }
        else if (read_bytes < BUFFER_SIZE) {
            result.append(buffer, read_bytes);
            break;
        }

        result.append(buffer, read_bytes);
    }

    return result;
}

NamedPipeServer::~NamedPipeServer() {
    close(pipe_fd_);
}

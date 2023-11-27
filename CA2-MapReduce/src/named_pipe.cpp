#include "../lib/named_pipe.hpp"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

#include <stdexcept>
#include <string>

void NamedPipe::remove_pipe(const std::string &pipe_path) {
    remove(pipe_path.c_str());
}

NamedPipeClient::NamedPipeClient(const std::string &pipe_name)
    : NamedPipe(pipe_name) {
    do
    {
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
    if ((pipe_fd_ = open(pipe_name.c_str(), O_RDONLY | O_NONBLOCK)) == -1)
    {
        if (mkfifo(pipe_name.c_str(), 0777) != 0) {
            throw std::runtime_error("FIFO creation failed.");
        }
        else
            pipe_fd_ = open(pipe_name_.c_str(), O_RDONLY | O_NONBLOCK);
    }
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

#ifndef __NAMED_PIPE_HPP__
#define __NAMED_PIPE_HPP__

#include <string>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

#include <stdexcept>
#include <string>

#include "types.hpp"
class NamedPipe {
public:
    NamedPipe(const std::string &pipe_name);

    static void remove_pipe(const std::string &pipe_path);

protected:
    std::string pipe_name_;
    int pipe_fd_;
};

class NamedPipeClient : NamedPipe {
    public:
        NamedPipeClient(const std::string &pipe_name);
        void send(const std::string &msg);
        ~NamedPipeClient();
};

class NamedPipeServer : NamedPipe {
    public:
        NamedPipeServer(const std::string &pipe_name);
        std::string receive();
        ~NamedPipeServer();
};

#endif

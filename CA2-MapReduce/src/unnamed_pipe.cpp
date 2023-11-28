#include "../lib/unnamed_pipe.hpp"

std::vector<int> create_pipe() {
    std::vector<int> pipe_fds(2);
    int fd[2];
    if (pipe(fd) == -1) {
        throw std::runtime_error("Pipe creating failed.");
    }
    pipe_fds[0] = fd[0];
    pipe_fds[1] = fd[1];
    return pipe_fds;
}

bool close_fd(int fd) {
    if (close(fd) == -1) {
        throw std::runtime_error("Pipe closing failed.");
    }

    return true;
}

int write_fd(const char *buffer, int size, int write_fd) {
    // std::cout << "WROTE: s" << 1 << std::endl;
    int bytes_written = write(write_fd, buffer, size);

    if (bytes_written == -1) {
        throw std::runtime_error("Writing to pipe failed.");
    }

    return bytes_written;
}

std::string read_fd(int read_fd) {
    char buffer[BUFFER_SIZE];
    std::string result;

    while (true) {
        int read_bytes = read(read_fd, buffer, BUFFER_SIZE);
        if (read_bytes == -1)
            if (errno == EAGAIN)
                break;
            
            else 
                throw std::runtime_error("Reading from pipe failed.");
            
        else if (read_bytes < BUFFER_SIZE) {
            result.append(buffer, read_bytes);
            break;
        }

        result.append(buffer, read_bytes);
    }
    return result;
}

#include <fstream>
#include <iostream>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bf_type;
    DWORD bf_size;
    WORD bf_reserved1;
    WORD bf_reserved2;
    DWORD bf_off_bits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD bi_size;
    LONG bi_width;
    LONG bi_height;
    WORD bi_planes;
    WORD bi_bit_count;
    DWORD bi_compression;
    DWORD bi_size_image;
    LONG bi_x_pels_per_meter;
    LONG bi_y_pels_per_meter;
    DWORD bi_clr_used;
    DWORD bi_clr_important;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

int rows;
int cols;

bool fill_and_allocate(char*& buffer, const char* file_name, int& rows, int& cols, int& buffer_size) {
    std::ifstream file(file_name);
    if (!file) {
        std::cout << "File " << file_name << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->bi_height;
    cols = info_header->bi_width;
    buffer_size = file_header->bf_size;
    return true;
}

void get_pixels_from_bmp24(int end, int rows, int cols, char* file_read_buffer) {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    // file_read_buffer[end - count] is the red value
                    break;
                case 1:
                    // file_read_buffer[end - count] is the green value
                    break;
                case 2:
                    // file_read_buffer[end - count] is the blue value
                    break;
                }
                // go to the next position in the buffer
            }
        }
    }
}

void write_out_bmp24(char* file_buffer, const char* name_of_file_to_create, int buffer_size) {
    std::ofstream write(name_of_file_to_create);
    if (!write) {
        std::cout << "Failed to write " << name_of_file_to_create << std::endl;
        return;
    }

    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    // write red value in file_buffer[buffer_size - count]
                    break;
                case 1:
                    // write green value in file_buffer[buffer_size - count]
                    break;
                case 2:
                    // write blue value in file_buffer[buffer_size - count]
                    break;
                }
                // go to the next position in the buffer
            }
        }
    }
    write.write(file_buffer, buffer_size);
}

int main(int argc, char* argv[]) {
    char* file_buffer;
    int buffer_size;
    if (!fill_and_allocate(file_buffer, argv[1], rows, cols, buffer_size)) {
        std::cout << "File read error" << std::endl;
        return 1;
    }

    // read input file
    // apply filters
    // write output file

    return 0;
}

#include <fstream>
#include <iostream>

#define OUTPUT_FILE "output.bmp"

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

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};


Pixel **photo;
char *file_buffer;
int buffer_size;
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

void get_pixels_from_bmp24() {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    photo[i][j].red = file_buffer[buffer_size - count];
                    break;
                case 1:
                    photo[i][j].green = file_buffer[buffer_size - count];
                    break;
                case 2:
                    photo[i][j].blue = file_buffer[buffer_size - count];
                    break;
                }
                count++;
            }
        }
    }
}

void write_out_bmp24() {
    std::ofstream write(OUTPUT_FILE);
    if (!write) {
        std::cout << "Failed to write " << OUTPUT_FILE << std::endl;
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
                    file_buffer[buffer_size - count] = photo[i][j].red;
                    break;
                case 1:
                    file_buffer[buffer_size - count] = photo[i][j].green;
                    break;
                case 2:
                    file_buffer[buffer_size - count] = photo[i][j].blue;
                    break;
                }
                count++;
            }
        }
    }
    write.write(file_buffer, buffer_size);
}

void alloc_photo() {
    photo = new Pixel*[rows];
    for (int i = 0; i < rows; i++)
        photo[i] = new Pixel[cols];
}

void init(char* input_file_name) {
    if (!fill_and_allocate(file_buffer, input_file_name, rows, cols, buffer_size)) {
        std::cout << "ERROR: reading input file failed" << std::endl;
        exit(1);
    }
    alloc_photo();
}

void flip_photo_filter() {
    for (int i = 0; i < rows / 2; i++) {
        for (int j = 0; j < cols; j++) {
            std::swap(photo[i][j], photo[rows - i - 1][j]);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file_name>" << std::endl;
        return 1;
    }
    init(argv[1]);

    get_pixels_from_bmp24();
    // apply filters
    flip_photo_filter();
    // end filters
    write_out_bmp24();

    return 0;
}

#include <fstream>
#include <iostream>
#include <chrono>
#include <pthread.h>

#define EQ(x, y) abs(x - y) <= 0.00001f
#define TIME() std::chrono::high_resolution_clock::now()
#define MILLISEC(x) std::chrono::duration_cast< std::chrono::duration<float, std::milli>> (x).count()
#define CLAMP(x, low, high) (x > high) ? high : (x < low) ? low : x;

constexpr char OUTPUT_FILE[] = "output.bmp";
constexpr int MAX_RGB_VALUE = 255;
constexpr int MIN_RGB_VALUE = 0;
constexpr int GAUSSIAN_BLUR_KERNEL[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
constexpr double NORMALIZE_FACTOR = 1.0/16.0;
constexpr int NUMBER_OF_THREADS = 8;

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(1)
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

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

Pixel **photo;
Pixel **prev;
char *file_buffer;
int buffer_size;
int rows;
int cols;
float slopes[3][2];

Pixel** make_photo() {
    Pixel** photo = new Pixel*[rows];
    for (int i = 0; i < rows; i++)
        photo[i] = new Pixel[cols];
    return photo;
}

void alloc_photo() {
    photo = make_photo();
}

void* save_prev_photo(void *tid) {
    long thread_id = (long)tid;
    int start_row = double(rows) / double(NUMBER_OF_THREADS) * thread_id;
    int end_row = double(rows) / double(NUMBER_OF_THREADS) * (thread_id + 1);
    for (int i = start_row; i < end_row; i++)
        for (int j = 0; j < cols; j++)
            prev[i][j] = photo[i][j];

    pthread_exit(NULL);
}

void fill_slopes() {
    int mid_row = rows / 2;
    int mid_col = cols / 2;
    float slope1 = -(float)(mid_row) / (float)(mid_col);
    float slope2 = -(float)(rows) / (float)(cols);
    slopes[0][0] = slope1; slopes[0][1] = mid_row;
    slopes[1][0] = slope1; slopes[1][1] = mid_row + rows;
    slopes[2][0] = slope2; slopes[2][1] = rows;
}

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

void* get_pixels_from_bmp24(void* tid) {
    long thread_id = (long)tid; 
    int start_row = double(rows) / double(NUMBER_OF_THREADS) * thread_id;
    int end_row = double(rows) / double(NUMBER_OF_THREADS) * (thread_id + 1);
    int extra = cols % 4;
    int count = (thread_id * (rows/NUMBER_OF_THREADS)) * (cols * 3 + extra) + 1;
    for (int i = start_row; i < end_row; i++) {
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
    pthread_exit(NULL);
}

void* write_out_bmp24(void* tid) {
    std::ofstream write(OUTPUT_FILE);
    if (!write) {
        std::cout << "Failed to write " << OUTPUT_FILE << std::endl;
        exit(1);
    }

    long thread_id = (long)tid; 
    int start_row = double(rows) / double(NUMBER_OF_THREADS) * thread_id;
    int end_row = double(rows) / double(NUMBER_OF_THREADS) * (thread_id + 1);
    int extra = cols % 4;
    int count = (thread_id * (rows/NUMBER_OF_THREADS)) * (cols * 3 + extra) + 1;
    for (int i = start_row; i < end_row; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    file_buffer[buffer_size - count] = photo[i][j].green;
                    break;
                case 1:
                    file_buffer[buffer_size - count] = photo[i][j].blue;
                    break;
                case 2:
                    file_buffer[buffer_size - count] = photo[i][j].red;
                    break;
                }
                count++;
            }
        }
    }
    write.write(file_buffer, buffer_size);
    pthread_exit(NULL);
}

void* flip_photo_filter(void* tid) {
    long thread_id = (long)tid; 
    int start_col = double(cols) / double(NUMBER_OF_THREADS) * thread_id;
    int end_col = double(cols) / double(NUMBER_OF_THREADS) * (thread_id + 1);
    for (int i = 0; i < rows / 2; i++)
        for (int j = start_col; j < end_col; j++)
            std::swap(photo[i][j], photo[rows - i - 1][j]);

    pthread_exit(NULL);
}


void* blur_photo_filter(void* tid) {
    long thread_id = (long)tid;
    int start_row = double(rows) / double(NUMBER_OF_THREADS) * thread_id;
    int end_row = double(rows) / double(NUMBER_OF_THREADS) * (thread_id + 1);

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < cols; j++) {
            int tmp_red = 0;
            int tmp_green = 0;
            int tmp_blue = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int x_neighbor = i + dx;
                    int y_neighbor = j + dy;
                    if (x_neighbor >= 0 && x_neighbor < rows && y_neighbor >= 0 && y_neighbor < cols) {
                        tmp_red += int(prev[x_neighbor][y_neighbor].red) * GAUSSIAN_BLUR_KERNEL[dx + 1][dy + 1] * NORMALIZE_FACTOR;
                        tmp_green += int(prev[x_neighbor][y_neighbor].green) * GAUSSIAN_BLUR_KERNEL[dx + 1][dy + 1] * NORMALIZE_FACTOR;
                        tmp_blue += int(prev[x_neighbor][y_neighbor].blue) * GAUSSIAN_BLUR_KERNEL[dx + 1][dy + 1] * NORMALIZE_FACTOR;
                    }
                }
            }
            photo[i][j].red = CLAMP(tmp_red, MIN_RGB_VALUE, MAX_RGB_VALUE);
            photo[i][j].green = CLAMP(tmp_green, MIN_RGB_VALUE, MAX_RGB_VALUE);
            photo[i][j].blue = CLAMP(tmp_blue, MIN_RGB_VALUE, MAX_RGB_VALUE);
        }
    }
    pthread_exit(NULL);
}

void* purple_haze_filter(void* tid) {
    long thread_id = (long)tid;
    int start_row = double(rows) / double(NUMBER_OF_THREADS) * thread_id;
    int end_row = double(rows) / double(NUMBER_OF_THREADS) * (thread_id + 1);

    for (int i = start_row; i < end_row; i++)
        for (int j = 0; j < cols; j++) {
            int new_red = 0.5 * double(photo[i][j].red) + 0.3 * double(photo[i][j].green) + 0.5 * double(photo[i][j].blue);
            int new_green =  0.16 * double(photo[i][j].red) + 0.5 * double(photo[i][j].green) + 0.16 * double(photo[i][j].blue);
            int new_blue = 0.6 * double(photo[i][j].red) + 0.2 * double(photo[i][j].green) + 0.8 * double(photo[i][j].blue);

            photo[i][j].red = CLAMP(new_red, MIN_RGB_VALUE, MAX_RGB_VALUE);
            photo[i][j].green = CLAMP(new_green, MIN_RGB_VALUE, MAX_RGB_VALUE);
            photo[i][j].blue = CLAMP(new_blue, MIN_RGB_VALUE, MAX_RGB_VALUE);
        }
        
  pthread_exit(NULL);
}

void* draw_lines_filter(void* tid) {
    int mid_row = rows / 2;
    int mid_col = cols / 2;
    long thread_id = (long)tid;

    int start_row = (thread_id == 0 || thread_id == 2) ? 0 : (mid_row);
    int end_row = (thread_id == 0) ? mid_row : rows;
    int start_col = (thread_id == 0 || thread_id == 2) ? 0 : (mid_col);
    int end_col = (thread_id == 0) ? (mid_col) : cols;

    for (int i = start_row; i < end_row; i++)
        for (int j = start_col; j < end_col; j++)
            if (EQ(j * slopes[thread_id][0] + slopes[thread_id][1], float(i))) {
                photo[i][j].red = MAX_RGB_VALUE;
                photo[i][j].green = MAX_RGB_VALUE;
                photo[i][j].blue = MAX_RGB_VALUE;
            }

    pthread_exit(NULL);
}


void handle_threads(void* (*func)(void*), int num_tr = NUMBER_OF_THREADS) {
    pthread_t threads[num_tr];
    int res;
    long t;
    for (t = 0; t < num_tr; t++) {
        res = pthread_create(&threads[t], NULL, func, (void *)t);
        if (res) {
            std::cout << "ERROR: creating thread failed --> " << res << std::endl;
            exit(-1);
        }
    }
    for (t = 0; t < num_tr; t++) {
        res = pthread_join(threads[t], NULL);
        if (res) {
            std::cout << "ERROR: joining threads failed --> " << res << std::endl;
            exit(-1);
        }
    }
}

void run() {
    auto read_file_start = TIME();
    handle_threads(get_pixels_from_bmp24);
    auto read_file_end = TIME();
    std::cout << "Read: " << MILLISEC(read_file_end - read_file_start) << " ms" << std::endl;

    auto flip_start = TIME();
    handle_threads(flip_photo_filter);
    auto flip_end = TIME();
    std::cout << "Flip: " << MILLISEC(flip_end - flip_start)  << " ms" << std::endl;

    handle_threads(save_prev_photo);

    auto blur_start = TIME();
    handle_threads(blur_photo_filter);
    auto blur_end = TIME();
    std::cout << "Blur: " << MILLISEC(blur_end - blur_start)  << " ms" << std::endl;

    auto purple_haze_start = TIME();
    handle_threads(purple_haze_filter);
    auto purple_haze_end = TIME();
    std::cout << "Purple: " << MILLISEC(purple_haze_end - purple_haze_start)  << " ms" << std::endl;

    auto draw_lines_start = TIME();
    handle_threads(draw_lines_filter, 3);
    auto draw_lines_end = TIME();
    std::cout << "Lines: " << MILLISEC(draw_lines_end - draw_lines_start)  << " ms" << std::endl;

    auto write_file_start = TIME();
    handle_threads(write_out_bmp24);
    auto write_file_end = TIME();
    std::cout << "Write: " << MILLISEC(write_file_end - write_file_start) << " ms" << std::endl;
}

void init(char* input_file_name) {
    if (!fill_and_allocate(file_buffer, input_file_name, rows, cols, buffer_size)) {
        std::cout << "ERROR: reading input file failed" << std::endl;
        exit(1);
    }
    alloc_photo();
    prev = make_photo();
    fill_slopes();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file_name>" << std::endl;
        return 1;
    }

    auto start = TIME();
    
    init(argv[1]);
    run();

    auto end = TIME();
    std::cout << "Execution: " << MILLISEC(end - start)  << " ms" << std::endl;

    return 0;
}

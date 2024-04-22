#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>
#include <iostream>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    char b = 'B';
    char m = 'M';
    uint32_t summary_size; // Суммарный размер заголовка и данных
    uint32_t reserved_space = 0; // Зарезервированное пространство
    uint32_t header_margain = 54; // Отступ данных от начала файла
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    uint32_t header_size = 40; // Размер заголовка
     int32_t width_in_pix; // Ширина изображения в пикселях
     int32_t height_in_pix; // Высота изображения в пикселях
    uint16_t planes_count = 1; // Количество плоскостей
    uint16_t bit_per_pix = 24; // Количество бит на пиксель
    uint32_t compression_type = 0; // Тип сжатия
    uint32_t byte_count; // Количество байт в данных
    uint32_t horisontal_resolution = 11811; // Горизонтальное разрешение, пикселей на метр
    uint32_t vertical_resolution = 11811; // Вертикальное разрешение, пикселей на метр
    uint32_t used_colors_count = 0; // Количество использованных цветов
    uint32_t significant_color_count = 0x1000000; // Количество значимых цветов
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    ofstream out(file, ios::binary);
    
    if (!out.is_open()){
        return false;
    }
    
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    
    // Calculating header parameters

    int stride = GetBMPStride(w);

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    file_header.summary_size = stride*h + 54;
    
    info_header.width_in_pix = w;
    info_header.height_in_pix = h;
    info_header.byte_count = stride*h;

    // Printing Header

    out.write(reinterpret_cast<char*>(&file_header),14);
    out.write(reinterpret_cast<char*>(&info_header),40);

    // Printing image
    std::vector<char> buff(stride,0);
    for (int y = h-1; y >= 0; --y){
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x){
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), stride);
    }    
    
    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);

    if (!ifs.is_open()){
        return {};
    }

    // Reading header

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
    ifs.read(reinterpret_cast<char*>(&file_header),14);
    ifs.read(reinterpret_cast<char*>(&info_header),40);

    if (file_header.b != 'B'                       || file_header.m != 'M' ||
        info_header.horisontal_resolution != 11811 || info_header.vertical_resolution != 11811) {
        return {};
    }
    

    // Loading image

    int w = info_header.width_in_pix;
    int h = info_header.height_in_pix;
    int stride = GetBMPStride(w);
    Image result(w, h, Color::Black());
    std::vector<char> buff(stride,0);

    for (int y = h-1; y >= 0; --y){
        Color* line = result.GetLine(y);
        ifs.read(buff.data(),stride);
        for (int x = 0; x < w; ++x){
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    } 
    return result;
}

}  // namespace img_lib
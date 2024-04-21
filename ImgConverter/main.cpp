#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>
#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class ImagePPMInterface : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return img_lib::SavePPM(file,image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadPPM(file);
    }
};

class ImageJPEGInterface : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return img_lib::SaveJPEG(file,image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadJPEG(file);
    }
};

class ImageBMPInterface : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return img_lib::SaveBMP(file,image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadBMP(file);
    }
};

enum class Format {
    JPEG,
    PPM,
    BMP,
    UNKNOWN
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    switch (GetFormatByExtension(path))
    {
    case Format::JPEG:
        return new ImageJPEGInterface();
        break;
    case Format::PPM:
        return new ImagePPMInterface();
    break;
    case Format::BMP:
        return new ImageBMPInterface();
    break;
    case Format::UNKNOWN:
        return nullptr;
    break;
    default:
        break;
    }
    return nullptr;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];
    
    auto input_interface = GetFormatInterface(in_path);
    auto output_interface = GetFormatInterface(out_path);
    if (input_interface == nullptr){
        cerr << "Unknown format of the input file"sv << endl;
        return 2;        
    }
    if (output_interface == nullptr){
        cerr << "Unknown format of the output file"sv << endl;
        return 3;        
    }
    img_lib::Image image = input_interface->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!output_interface->SaveImage(out_path,image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}
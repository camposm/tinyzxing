#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "ImageReaderSource.h"
#include <zxing/Binarizer.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
//#include <zxing/common/HybridBinarizer.h>
#include <exception>
#include <zxing/BinaryBitmap.h>
#include <zxing/qrcode/QRCodeReader.h>
#include "jpgd.h"
#include <stdio.h>

using namespace std;
using namespace zxing;
using namespace zxing::qrcode;


extern "C" {
    
void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));

static int64_t max_stack_usage = 0;
static int64_t stack_start = 0;

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
    volatile uint64_t ptr = 0;
    int64_t cur_stack_usage = stack_start - (uint64_t)&ptr;
    
    //printf("> %016lx\n", cur_stack_usage);
    if (cur_stack_usage > max_stack_usage) 
    {
        max_stack_usage = cur_stack_usage;
    }
}

void __cyg_profile_func_exit (void *this_fn, void *call_site)
{
    // blank
}

}

int read_image(LuminanceSource& source, bool hybrid) 
{
    auto global_bin = GlobalHistogramBinarizer(source);
    BinaryBitmap binary(global_bin);
    QRCodeReader qr_reader;
    
    auto result = qr_reader.decode(binary);

    std::cout << (hybrid ? "Hybrid" : "Global") << " binarizer ";
    if (result.second != 0)
    {
        std::cout << "succeded\n";
        
        /*
        std::cout << "  Format: " << BarcodeFormat::barcodeFormatNames[result.value().getBarcodeFormat()] << "\n";
        for (int j = 0; j < result.value().getResultPoints().size(); j++) 
        {
            auto x = result.value().getResultPoints()[j].first;
            if (x >= 0.0)
            {
                std::cout << "  Point[" << j <<  "]: " << x << " " << result.value().getResultPoints()[j].second << "\n";
            }
        }
        */
        std::cout << "[";
        for (auto n = 0; n < result.second; ++n) 
        {
            std::cout << result.first[n];
        }
        std::cout << "]\n";
        
        return 0;
    }
    else
    {
        std::cout << "failed\n";
        return -1;
    }
    
    return 0;
}

std::pair<char*, int> load_image (const std::string& filename)
{
    string extension = filename.substr(filename.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  
    int width = 0;
    int height = 0;
    int channels = 0;

    char* buffer = nullptr;
  
    if (extension == "jpg" || extension == "jpeg") 
    {
        buffer = reinterpret_cast<char*>(
            jpgd::decompress_jpeg_image_from_file(
                filename.c_str(), 
                &width, 
                &height, 
                &channels, 
                4
            )
        );
    }
    
    printf("w=%d h=%d ch=%d\n", width, height, channels);
    
    
    if (width != zxing::config::ImageWidth || height != zxing::config::ImageHeight)
    {
        std::cout << "e: image size mismatch for testing - expected ";
        std::cout << zxing::config::ImageWidth << "x" << zxing::config::ImageHeight << ", got ";
        std::cout << width << "x" << height << "\n";
        return {};
    }
    
    if (buffer == nullptr) 
    {
        std::cout << "e: failed to load " << filename << "\n";
        return {};
    }
    
    return {buffer, channels};
}

int main(int argc, char** argv) 
{
    volatile uint64_t start = 0;
    
    stack_start = (int64_t)&start;
    
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " input.jpg\n";
        return 0;
    }

    //printf("stack start = %016lx\n", stack_start);

    auto image_meta = load_image(argv[1]);
    printf("image_meta = (%p, %d)\n", image_meta.first, image_meta.second);
    
    if (image_meta.first == nullptr)
    {
        return 0;
    }
    
    ImageReaderSource  source(image_meta.first, image_meta.second);
    
    free(image_meta.first);
    
    std::cout << "global: ";
    auto result = read_image(source, false); // global
    if (result != 0)
    {
        std::cout << "FAIL (" << result << ")\n";
    }
    else
    {
        std::cout << "OK!\n";
    }
    
    //std::cout << "max_stack_usage = " << max_stack_usage << " bytes\n";
    
    return 0;
}

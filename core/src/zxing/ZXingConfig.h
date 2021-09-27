#ifndef __ZXINGCONFIG_H__
#define __ZXINGCONFIG_H__

#include <cstdint>
#include <array>

namespace zxing {
namespace config {

constexpr uint32_t  ImageWidth = 320;
constexpr uint32_t  ImageHeight = 240;

constexpr uint32_t  MaxGridSize = 49;
constexpr uint32_t  MaxSupportedQRVersion = (MaxGridSize - 17) / 4;

constexpr uint32_t  MaxRawBytes = 256; // TODO: must be calculated according to MaxSupportedQRVersion
using rawbytes_t = std::pair<std::array<char, MaxRawBytes>, uint32_t>;

constexpr uint32_t  MaxDecodedOutput = 256; // TODO: must be calculated according to something (?)
using decoded_output_t = std::pair<std::array<char, MaxDecodedOutput>, uint32_t>;

constexpr uint32_t some_number_of_codewords = 64; // TODO: must be calculated according to something (?)


using gs_points_t = std::array<float, (config::MaxGridSize << 1)>;

constexpr size_t totalCodewordsPerVersion[] = {
    26,   44,   70,   100,  134,  172,  196,  242,  292,  346, 
    404,  466,  532,  581,  655,  733,  815,  901,  991,  1085, 
    1156, 1258, 1364, 1474, 1588, 1706, 1828, 1921, 2051, 2185, 
    2323, 2465, 2611, 2761, 2876, 3034, 3196, 3362, 3532, 3706
};

constexpr size_t totalDataBlocksPerVersion[] = {
    1,  1,  2,  4,  4,  4,  5,  6,  8,  8, 11, 11, 16, 16, 18, 16, 19, 
    21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60
};

using codewords_t = std::array<char, totalCodewordsPerVersion[MaxSupportedQRVersion - 1]>;


constexpr uint32_t  MaxPossibleCenters = 8;


} // ns config
} // ns zxing

#endif // __ZXINGCONFIG_H__

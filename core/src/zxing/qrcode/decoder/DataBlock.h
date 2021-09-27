#ifndef __DATA_BLOCK_H__
#define __DATA_BLOCK_H__

/*
 *  DataBlock.h
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <array>
#include <optional>
#include <zxing/ZXingConfig.h>
#include <zxing/qrcode/Version.h>
#include <zxing/qrcode/ErrorCorrectionLevel.h>


namespace zxing {
namespace qrcode {

class DataBlock;

using datablocks_t = std::array<
    DataBlock, 
    config::totalDataBlocksPerVersion[config::MaxSupportedQRVersion - 1]
>;

struct DataBlock
{
    DataBlock() = default;
    
    static std::optional<datablocks_t> getDataBlocks (const config::codewords_t& rawCodewords, const Version* version, int ecLevel);

    int numDataCodewords_;
    int numBlockCodewords_;
    std::array<char, zxing::config::some_number_of_codewords> codewords_;
};

} // ns qrcode
} // ns zxing

#endif // __DATA_BLOCK_H__

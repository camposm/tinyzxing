#ifndef __BIT_MATRIX_PARSER_H__
#define __BIT_MATRIX_PARSER_H__

/*
 *  BitMatrixParser.h
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

#include <optional>
#include <zxing/common/BitMatrix.h>
#include <zxing/ZXingConfig.h>
#include <zxing/common/GridSampler.h>
#include <zxing/qrcode/Version.h>
#include <zxing/qrcode/FormatInformation.h>


namespace zxing {
namespace qrcode {

class BitMatrixParser
{

public:
    explicit BitMatrixParser (sampled_grid_t& bitMatrix);
    std::optional<FormatInformation> readFormatInformation();
    const Version* readVersion();
    codewords_t readCodewords();

private:
    sampled_grid_t& bitMatrix_;
    bool found_error;

    const Version* parsedVersion_;
    std::optional<FormatInformation> parsedFormatInfo_;

    int copyBit (size_t x, size_t y, int versionBits);
  
};

} // ns qrcode
} // ns zxing

#endif // __BIT_MATRIX_PARSER_H__

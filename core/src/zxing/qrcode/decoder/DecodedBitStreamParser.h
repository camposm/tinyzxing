#ifndef __DECODED_BIT_STREAM_PARSER_H__
#define __DECODED_BIT_STREAM_PARSER_H__

/*
 *  DecodedBitStreamParser.h
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

#include <zxing/ZXingConfig.h>
#include <zxing/qrcode/decoder/Mode.h>
#include <zxing/common/BitSource.h>


namespace zxing {
namespace qrcode {

class DecodedBitStreamParser 
{
public:
    static config::decoded_output_t decode(
        std::array<char, config::MaxDecodedOutput>& bytes, 
        const int input_size, 
        const Version* version, 
        const int ecLevel
    );
};

} // ns qrcode
} // ns zxing

#endif // __DECODED_BIT_STREAM_PARSER_H__

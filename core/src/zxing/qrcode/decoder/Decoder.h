#ifndef __DECODER_H__
#define __DECODER_H__

/*
 *  Decoder.h
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
#include <zxing/common/reedsolomon/ReedSolomonDecoder.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/common/GridSampler.h>
#include <zxing/qrcode/decoder/DataBlock.h>

namespace zxing {
namespace qrcode {

class Decoder 
{
public:
    Decoder();
    config::decoded_output_t decode(sampled_grid_t& bits);

private:
    ReedSolomonDecoder rsDecoder_;

};

} // ns qrcode
} // ns zxing

#endif // __DECODER_H__

#ifndef __GLOBALHISTOGRAMBINARIZER_H__
#define __GLOBALHISTOGRAMBINARIZER_H__
/*
 *  GlobalHistogramBinarizer.h
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

#include <zxing/Binarizer.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/ZXingConfig.h>

namespace zxing {
namespace {
    
constexpr int LUMINANCE_BITS = 5;
constexpr int LUMINANCE_SHIFT = 8 - LUMINANCE_BITS;
constexpr int LUMINANCE_BUCKETS = 1 << LUMINANCE_BITS;

} // ns -anon-

class GlobalHistogramBinarizer : public Binarizer 
{
public:
    explicit GlobalHistogramBinarizer(LuminanceSource& source);
    virtual ~GlobalHistogramBinarizer() = default;
       
    const bitmatrix_i& getBlackMatrix();

    int estimateBlackPoint();

private:
    std::array<int, LUMINANCE_BUCKETS> buckets;
    bitmatrix_t<config::ImageWidth, config::ImageHeight> matrix;

};

} // ns zxing

#endif /* GLOBALHISTOGRAMBINARIZER_H_ */

#ifndef __REED_SOLOMON_DECODER_H__
#define __REED_SOLOMON_DECODER_H__

/*
 *  ReedSolomonDecoder.h
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

#include <utility>
#include <zxing/ZXingConfig.h>
#include <zxing/common/reedsolomon/GenericGFPoly.h>
#include <zxing/common/reedsolomon/GenericGF.h>


namespace zxing {


class ReedSolomonDecoder 
{
public:
    explicit ReedSolomonDecoder (const GenericGF_i& field);
    
    bool decode (std::array<char, zxing::config::some_number_of_codewords>& received, const int numCodewords, const int twoS);

private:
    const GenericGF_i& field;
    
    std::pair<zxing::my_GenericGFPoly, zxing::my_GenericGFPoly> runEuclideanAlgorithm(
        const zxing::my_GenericGFPoly& a, 
        const zxing::my_GenericGFPoly& b, 
        int R
    );
    
    struct error_result
    {
        std::array<int, max_res_size>   values;
        int                             size;
    };

    error_result findErrorLocations(const zxing::my_GenericGFPoly& errorLocator);
    error_result findErrorMagnitudes(
        const zxing::my_GenericGFPoly& errorEvaluator, 
        const error_result& errorLocations
    );
};

} // ns zxing

#endif // __REED_SOLOMON_DECODER_H__

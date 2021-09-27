/*
 *  DataMask.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 19/05/2008.
 *  Copyright 2008 ZXing authors All rights reserved.
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

#include <zxing/qrcode/decoder/DataMask.h>


namespace zxing {
namespace qrcode {

std::optional<DataMask> DataMask::select (int reference)
{
    switch (reference)
    {
        case 0: return DataMask([](size_t x, size_t y) {
            return ((x + y) % 2) == 0; 
        });
        case 1: return DataMask([](size_t x, size_t y) {
            return (x % 2) == 0;
        });
        case 2: return DataMask([](size_t x, size_t y) {
            return y % 3 == 0;
        });
        case 3: return DataMask([](size_t x, size_t y) {
            return (x + y) % 3 == 0;
        });
        case 4: return DataMask([](size_t x, size_t y) {
            return (((x >> 1) + (y / 3)) % 2) == 0;
        });
        case 5: return DataMask([](size_t x, size_t y) {
            size_t temp = x * y;
            return (temp % 2) + (temp % 3) == 0;
        });
        case 6: return DataMask([](size_t x, size_t y) {
            size_t temp = x * y;
            return (((temp % 2) + (temp % 3)) % 2) == 0;
        });
        case 7: return DataMask([](size_t x, size_t y) {
            return ((((x + y) % 2) + ((x * y) % 3)) % 2) == 0;
        });
        default: return {};
    }
}

void DataMask::unmaskBitMatrix (sampled_grid_t& bits, size_t dimension) 
{
    for (size_t y = 0; y < dimension; y++) 
    {
        for (size_t x = 0; x < dimension; x++) 
        {
            // TODO: check why the coordinates have to be swapped
            if (isMasked(y, x)) 
            {
                bits.flip(x, y);
            }
        }
    }
}

} // ns qrcode
} // ns zxing

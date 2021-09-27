#ifndef __LUMINANCESOURCE_H__
#define __LUMINANCESOURCE_H__
/*
 *  LuminanceSource.h
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
#include <zxing/ZXingConfig.h>


namespace zxing {

using lum_matrix_t  = std::array<char, config::ImageWidth * config::ImageHeight>;
using lum_row_t     = std::array<char, config::ImageWidth>;

class LuminanceSource
{
public:
    LuminanceSource() = default;
    virtual ~LuminanceSource() = default;

    //virtual lum_row_t&       getRow (int y, lum_row_t& row) const = 0; // ??
    virtual const lum_row_t& getRow (int y) const = 0;
    
    //virtual lum_matrix_t&       getMatrix() const = 0; // ??
    virtual const lum_matrix_t& getMatrix() const = 0;

    virtual bool isRotateSupported() const 
    {
        return false;
    }
    
};

} // ns zxing

#endif /* LUMINANCESOURCE_H_ */

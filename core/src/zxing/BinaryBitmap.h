#ifndef __BINARYBITMAP_H__
#define __BINARYBITMAP_H__

/*
 *  BinaryBitmap.h
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

#include <zxing/common/BitMatrix.h>
#include <zxing/Binarizer.h>

namespace zxing {

class BinaryBitmap
{
public:
    explicit BinaryBitmap (Binarizer& binarizer) : 
        binarizer_(binarizer) 
    {
        // blank
    }
    
    virtual ~BinaryBitmap() = default;
       
    const bitmatrix_i& getBlackMatrix()
    {
        return binarizer_.getBlackMatrix();
    }

    LuminanceSource& getLuminanceSource() const
    {
        return binarizer_.getLuminanceSource();
    }

private:
    Binarizer& binarizer_;

};

}

#endif /* BINARYBITMAP_H_ */

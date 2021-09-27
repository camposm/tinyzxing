#ifndef BINARIZER_H_
#define BINARIZER_H_

/*
 *  Binarizer.h
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

#include <zxing/LuminanceSource.h>
#include <zxing/common/BitMatrix.h>


namespace zxing {

class Binarizer
{
public:
    Binarizer(LuminanceSource& source) : 
        source_(source) 
    {
        // blank
    }
    
    virtual ~Binarizer() = default;

    virtual const bitmatrix_i& getBlackMatrix() = 0;

    LuminanceSource& getLuminanceSource() const
    {
        return source_;
    }

private:
    LuminanceSource& source_;

};

} // ns zxing

#endif /* BINARIZER_H_ */

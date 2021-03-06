#ifndef __READER_H__
#define __READER_H__

/*
 *  Reader.h
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
#include <zxing/BinaryBitmap.h>

namespace zxing {

class Reader 
{
public:
    virtual config::decoded_output_t decode(BinaryBitmap& image)
    {
        return decode(image);
    }

    virtual ~Reader() = default;
   
protected:
    Reader() = default;
   
};

} // ns zxing

#endif // __READER_H__

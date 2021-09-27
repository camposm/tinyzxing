#ifndef __IMAGE_READER_SOURCE_H_
#define __IMAGE_READER_SOURCE_H_
/*
 *  Copyright 2010-2011 ZXing authors
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
#include <zxing/ZXingConfig.h>


class ImageReaderSource : public zxing::LuminanceSource 
{
public:
    ImageReaderSource (const char* source, int channels);


    void load_image (const char* source, int channels);

    const zxing::lum_row_t& getRow (int y) const;
    const zxing::lum_matrix_t& getMatrix() const;

private:
    zxing::lum_matrix_t image;
    
};

#endif /* __IMAGE_READER_SOURCE_H_ */

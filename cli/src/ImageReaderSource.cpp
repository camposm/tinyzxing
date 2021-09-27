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

#include "ImageReaderSource.h"
#include <iostream>
#include <cstdlib>
#include <iterator>
#include <algorithm>


using zxing::LuminanceSource;


ImageReaderSource::ImageReaderSource (const char* source, int channels) : 
    LuminanceSource(), 
    image{0}
{
    load_image(source, channels);
}

void ImageReaderSource::load_image (const char* source, int channels)
{
    switch (channels)
    {
        case 1: // gray
        case 2: // gray + alpha
        {
            uint32_t counter = 0;
            auto end = source + (zxing::config::ImageWidth * zxing::config::ImageHeight * channels);
            
            std::copy_if(
                source, end, image.begin(),
                [&counter,&channels](auto& e)
                {
                    static_cast<void>(e);
                    return (counter++) % channels == 0;
                }
            );
            break;
        }
        case 3: // rgb
        case 4: // rgba
        {
            const auto total = zxing::config::ImageWidth * zxing::config::ImageHeight;
            const char* pixel = source;
            
            for (auto n = 0; n < total; ++n)
            {
                image[n] = (char)((306 * (int) pixel[0] + 601 * (int)pixel[1] +  117 * (int)pixel[2] + 0x200) >> 10);
                //pixel += channels;
                pixel += 4;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    
    FILE* f = fopen("wololo", "w");
    if (f)
    {
        for (int i = 0; i < zxing::config::ImageHeight; ++i)
        {
            for (int j = 0; j < zxing::config::ImageWidth; ++j)
            {
                fprintf(f, "%d ", (int)image[i * zxing::config::ImageWidth + j]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }
    
    
}

const zxing::lum_row_t& ImageReaderSource::getRow (int row_no) const 
{
    auto data = image.data() + row_no * zxing::config::ImageWidth;
    return *reinterpret_cast<const zxing::lum_row_t*>(data);
}

const zxing::lum_matrix_t& ImageReaderSource::getMatrix() const 
{
    return image;
}

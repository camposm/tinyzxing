#ifndef __BIT_MATRIX_H__
#define __BIT_MATRIX_H__

/*
 *  BitMatrix.h
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

#include <bitset>
#include <limits>


namespace zxing {

class bitmatrix_i
{
public:
    virtual ~bitmatrix_i() = default;
    
    virtual bool get (uint32_t x, uint32_t y) const = 0;
    virtual void set (uint32_t x, uint32_t y) = 0;
    virtual void flip (uint32_t x, uint32_t y) = 0;
    virtual void clear() = 0;
    virtual void setRegion (uint32_t left, uint32_t top, uint32_t width, uint32_t height) = 0;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
};


template <uint32_t Width, uint32_t Height>
class bitmatrix_t : public bitmatrix_i
{
public:
    explicit bitmatrix_t (uint32_t reduced_dimension) : 
        r_width(reduced_dimension),
        r_height(reduced_dimension)
    {
        // blank
    }

    bitmatrix_t() : 
        r_width(0),
        r_height(0)
    {
        // blank
    }

    bool get (uint32_t x, uint32_t y) const 
    {
        if (x > Width || y > Height)
        {
            return false;
        }
        
        return data[y * Width + x];
    }

    void set (uint32_t x, uint32_t y) 
    {
        if (x < Width && y < Height)
        {
            data[y * Width + x] = 1;
        }
    }
    
    void flip (uint32_t x, uint32_t y)
    {
        if (x < Width && y < Height)
        {        
            data.flip(y * Width + x);
        }
    }
    
    void clear()
    {
        data.reset();
    }
    
    void setRegion (uint32_t left, uint32_t top, uint32_t width, uint32_t height)
    {
        for (uint32_t j = 0; j < height; ++j)
        {
            for (uint32_t i = 0; i < width; ++i)
            {
                auto idx = (top + j) * Width + (left + i);
                if (idx < (Width * Height))
                {
                    data[idx] = 1;
                }
            }
        }
    }
    
    uint32_t getWidth() const
    {
        return (r_width != 0) ? r_width : Width;
    }
    
    uint32_t getHeight() const
    {
        return (r_height != 0) ? r_height : Height;
    }
    
private:
    std::bitset<Width * Height> data;
    uint32_t r_width;
    uint32_t r_height;

};

} // ns zxing

#endif // __BIT_MATRIX_H__

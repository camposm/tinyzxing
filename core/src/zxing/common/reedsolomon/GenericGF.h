/*
 *  GenericGF.h
 *  zxing
 *
 *  Created by Lukas Stabe on 13/02/2012.
 *  Copyright 2012 ZXing authors All rights reserved.
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

#ifndef GENERICGF_H
#define GENERICGF_H

#include <cstdint>
#include <array>


namespace zxing {
    
class GenericGF_i
{
public:
    virtual ~GenericGF_i() = default;
    
    virtual int exp (int a) const = 0;
    virtual int log (int a) const = 0;  
    virtual int inverse (int a) const = 0;
    virtual int multiply (int a, int b) const = 0;
    virtual int getSize() const = 0;
    virtual int getGeneratorBase() const = 0;
};

const GenericGF_i& get_QR_Field();


template <uint32_t Primitive, uint32_t Size, uint32_t Base>
struct my_GenericGF : public GenericGF_i
{
    constexpr my_GenericGF() :
        expTable{0}, logTable{0}
    {
        int x = 1;
        for (int i = 0; i < Size; i++) 
        {
            expTable[i] = x;
            x <<= 1; // x = x * 2; we're assuming the generator alpha is 2
            if (x >= Size) 
            {
                x ^= Primitive;
                x &= (Size - 1);
            }
        }
        
        for (int i = 0; i < Size-1; i++) 
        {
            logTable[expTable[i]] = i;
        }
    }
  
    int exp (int a) const
    {
        if (a >= Size)
        {
            return -1;
        }
        return expTable[a];
    }
  
    int log (int a) const
    {
        if (a >= Size)
        {
            return -1;
        }
        return logTable[a];
    }
  
    int inverse (int a) const
    {
        if (a >= Size)
        {
            return -1;
        }
        return expTable[Size - logTable[a] - 1];
    }
  
    int multiply (int a, int b) const
    {
        if (a == 0 || b == 0) 
        {
            return 0;
        }
    
        return expTable[(logTable[a] + logTable[b]) % (Size - 1)];
    }
    
    int getSize() const
    {
        return Size;
    }

    int getGeneratorBase() const
    {
        return Base;
    }

    std::array<int, Size>   expTable;
    std::array<int, Size>   logTable;
};

} // ns zxing

#endif //GENERICGF_H


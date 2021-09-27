#ifndef __BIT_SOURCE_H__
#define __BIT_SOURCE_H__

/*
 *  BitSource.h
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

namespace zxing {
    
template <int MaxSize>
class bitsource_t
{
public:
    bitsource_t (const std::array<char, MaxSize>& bytes, const int input_size) : 
        src(bytes), input_size(input_size), byte_offset(0), bit_offset(0)
    {
        // blank
    }
    
    int readBits (int numBits)
    {
        if (numBits < 0 || numBits > 32)
        {
            return 0;
        }
        
        if (numBits > availableBits())
        {
            numBits = availableBits();
        }
        
        int result = 0;
        
        // First, read remainder from current byte
        if (bit_offset > 0) 
        {
            int bitsLeft = 8 - bit_offset;
            int toRead = numBits < bitsLeft ? numBits : bitsLeft;
            int bitsToNotRead = bitsLeft - toRead;
            int mask = (0xFF >> (8 - toRead)) << bitsToNotRead;
            
            result = (src[byte_offset] & mask) >> bitsToNotRead;
            numBits -= toRead;
            bit_offset += toRead;
            
            if (bit_offset == 8) 
            {
                bit_offset = 0;
                byte_offset++;
            }
        }

        // Next read whole bytes
        if (numBits > 0) 
        {
            while (numBits >= 8) 
            {
                result = (result << 8) | (src[byte_offset] & 0xFF);
                byte_offset++;
                numBits -= 8;
            }
            // Finally read a partial byte
            if (numBits > 0) 
            {
                int bitsToNotRead = 8 - numBits;
                int mask = (0xFF >> bitsToNotRead) << bitsToNotRead;
                result = (result << numBits) | ((src[byte_offset] & mask) >> bitsToNotRead);
                bit_offset += numBits;
            }
        }

        return result;
    }
    
    int getBitOffset() const
    {
        return bit_offset;
    }

    int getByteOffset() const
    {
        return byte_offset;
    }
     
    int availableBits() const
    {
        return 8 * (input_size - byte_offset) - bit_offset;
    }
    
private:    
    const std::array<char, MaxSize>& src;
    const int input_size;
    int byte_offset;
    int bit_offset;
};

} // ns zxing

#endif // __BIT_SOURCE_H__

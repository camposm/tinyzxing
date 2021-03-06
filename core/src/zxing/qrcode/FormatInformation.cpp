/*
 *  FormatInformation.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 18/05/2008.
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

#include <limits>
#include <zxing/qrcode/FormatInformation.h>


namespace zxing {
namespace qrcode {

const int FormatInformation::FORMAT_INFO_MASK_QR = 0x5412;
const int FormatInformation::FORMAT_INFO_DECODE_LOOKUP[][2] = { 
    { 0x5412, 0x00 }, { 0x5125, 0x01 }, { 0x5E7C, 0x02 }, { 0x5B4B, 0x03 }, { 0x45F9, 0x04 }, { 0x40CE, 0x05 }, 
    { 0x4F97, 0x06 }, { 0x4AA0, 0x07 }, { 0x77C4, 0x08 }, { 0x72F3, 0x09 }, { 0x7DAA, 0x0A }, { 0x789D, 0x0B }, 
    { 0x662F, 0x0C }, { 0x6318, 0x0D }, { 0x6C41, 0x0E }, { 0x6976, 0x0F }, { 0x1689, 0x10 }, { 0x13BE, 0x11 }, 
    { 0x1CE7, 0x12 }, { 0x19D0, 0x13 }, { 0x0762, 0x14 }, { 0x0255, 0x15 }, { 0x0D0C, 0x16 }, { 0x083B, 0x17 }, 
    { 0x355F, 0x18 }, { 0x3068, 0x19 }, { 0x3F31, 0x1A }, { 0x3A06, 0x1B }, { 0x24B4, 0x1C }, { 0x2183, 0x1D }, 
    { 0x2EDA, 0x1E }, { 0x2BED, 0x1F },
};

const int FormatInformation::N_FORMAT_INFO_DECODE_LOOKUPS = 32;
const int FormatInformation::BITS_SET_IN_HALF_BYTE[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

FormatInformation::FormatInformation (int formatInfo) :
    dataMask_((char)(formatInfo & 0x07)) 
{
    auto level = (formatInfo >> 3) & 0x03;
    
    if (level < 4)
    {
        errorCorrectionLevel_ = ErrorCorrectionLevel[level];
    }
    else
    {
        errorCorrectionLevel_ = -1;
    }
}

int FormatInformation::getErrorCorrectionLevel() const
{
    return errorCorrectionLevel_;
}

char FormatInformation::getDataMask() const
{
    return dataMask_;
}

int FormatInformation::numBitsDiffering (int a, int b) 
{
    a ^= b;
    return BITS_SET_IN_HALF_BYTE[a & 0x0F] + 
        BITS_SET_IN_HALF_BYTE[(a >> 4 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 8 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 12 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 16 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 20 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 24 & 0x0F)] + 
        BITS_SET_IN_HALF_BYTE[(a >> 28 & 0x0F)];
}

std::optional<FormatInformation> FormatInformation::decodeFormatInformation (int maskedFormatInfo1, int maskedFormatInfo2) 
{
    auto result = doDecodeFormatInformation(maskedFormatInfo1, maskedFormatInfo2);
    
    if (result) 
    {
        return result;
    }
    
    // Should return null, but, some QR codes apparently
    // do not mask this info. Try again by actually masking the pattern
    // first
    return doDecodeFormatInformation(
        maskedFormatInfo1 ^ FORMAT_INFO_MASK_QR,
        maskedFormatInfo2  ^ FORMAT_INFO_MASK_QR
    );
}

std::optional<FormatInformation> FormatInformation::doDecodeFormatInformation (int maskedFormatInfo1, int maskedFormatInfo2) 
{
    // Find the int in FORMAT_INFO_DECODE_LOOKUP with fewest bits differing
    int bestDifference = std::numeric_limits<int>::max();
    int bestFormatInfo = 0;
    
    for (int i = 0; i < N_FORMAT_INFO_DECODE_LOOKUPS; i++) 
    {
        const int* decodeInfo = FORMAT_INFO_DECODE_LOOKUP[i];
        int targetInfo = decodeInfo[0];
    
        if (targetInfo == maskedFormatInfo1 || targetInfo == maskedFormatInfo2) 
        {
            // Found an exact match
            return FormatInformation(decodeInfo[1]);
        }
    
        int bitsDifference = numBitsDiffering(maskedFormatInfo1, targetInfo);

        if (bitsDifference < bestDifference) 
        {
            bestFormatInfo = decodeInfo[1];
            bestDifference = bitsDifference;
        }
        
        if (maskedFormatInfo1 != maskedFormatInfo2) 
        {
            // also try the other option
            bitsDifference = numBitsDiffering(maskedFormatInfo2, targetInfo);
            
            if (bitsDifference < bestDifference) 
            {
                bestFormatInfo = decodeInfo[1];
                bestDifference = bitsDifference;
            }
        }
    }
    
    if (bestDifference <= 3) 
    {
        return FormatInformation(bestFormatInfo);
    }
    
    return {};
}

bool operator== (const FormatInformation& a, const FormatInformation& b) 
{
    return &(a.errorCorrectionLevel_) == &(b.errorCorrectionLevel_) && a.dataMask_ == b.dataMask_;
}

} // ns qrcode
} // ns zxing

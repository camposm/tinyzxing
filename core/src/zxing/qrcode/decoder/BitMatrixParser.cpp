/*
 *  BitMatrixParser.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 20/05/2008.
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

#include <zxing/qrcode/decoder/BitMatrixParser.h>
#include <zxing/qrcode/decoder/DataMask.h>


namespace zxing {
namespace qrcode {

int BitMatrixParser::copyBit(size_t x, size_t y, int versionBits) {
  return bitMatrix_.get(x, y) ? (versionBits << 1) | 0x1 : versionBits << 1;
}

BitMatrixParser::BitMatrixParser (sampled_grid_t& bitMatrix) :
    bitMatrix_(bitMatrix), 
    found_error(false),
    parsedVersion_(0), 
    parsedFormatInfo_()
{
    size_t dimension = bitMatrix.getHeight();
    if ((dimension < 21) || (dimension & 0x03) != 1) 
    {
        found_error = true;
    }
}

std::optional<FormatInformation> BitMatrixParser::readFormatInformation() 
{
    if (parsedFormatInfo_) 
    {
        return parsedFormatInfo_;
    }

    // Read top-left format info bits
    int formatInfoBits1 = 0;
    for (int i = 0; i < 6; i++) 
    {
        formatInfoBits1 = copyBit(i, 8, formatInfoBits1);
    }
    
    // .. and skip a bit in the timing pattern ...
    formatInfoBits1 = copyBit(7, 8, formatInfoBits1);
    formatInfoBits1 = copyBit(8, 8, formatInfoBits1);
    formatInfoBits1 = copyBit(8, 7, formatInfoBits1);
    // .. and skip a bit in the timing pattern ...
    for (int j = 5; j >= 0; j--) 
    {
        formatInfoBits1 = copyBit(8, j, formatInfoBits1);
    }

    // Read the top-right/bottom-left pattern
    int dimension = bitMatrix_.getHeight();
    int formatInfoBits2 = 0;
    int jMin = dimension - 7;
    
    for (int j = dimension - 1; j >= jMin; j--) 
    {
        formatInfoBits2 = copyBit(8, j, formatInfoBits2);
    }
    
    for (int i = dimension - 8; i < dimension; i++) 
    {
        formatInfoBits2 = copyBit(i, 8, formatInfoBits2);
    }

    return FormatInformation::decodeFormatInformation(formatInfoBits1,formatInfoBits2);
}

const Version* BitMatrixParser::readVersion() 
{
    if (parsedVersion_ != nullptr) 
    {
        return parsedVersion_;
    }

    int dimension = bitMatrix_.getHeight();

    int provisionalVersion = (dimension - 17) >> 2;
    if (provisionalVersion <= 6) 
    {
        return Version::getVersionForNumber(provisionalVersion);
    }

    // Read top-right version info: 3 wide by 6 tall
    int versionBits = 0;
    for (int y = 5; y >= 0; y--) 
    {
        int xMin = dimension - 11;
        for (int x = dimension - 9; x >= xMin; x--) 
        {
            versionBits = copyBit(x, y, versionBits);
        }
    }

    parsedVersion_ = Version::decodeVersionInformation(versionBits);
    if (parsedVersion_ != nullptr && parsedVersion_->getDimensionForVersion() == dimension) 
    {
        return parsedVersion_;
    }

    // Hmm, failed. Try bottom left: 6 wide by 3 tall
    versionBits = 0;
    for (int x = 5; x >= 0; x--) 
    {
        int yMin = dimension - 11;
        for (int y = dimension - 9; y >= yMin; y--) 
        {
            versionBits = copyBit(x, y, versionBits);
        }
    }

    parsedVersion_ = Version::decodeVersionInformation(versionBits);
    if (parsedVersion_ != nullptr && parsedVersion_->getDimensionForVersion() == dimension) 
    {
        return parsedVersion_;
    }
  
    return nullptr;
}

codewords_t BitMatrixParser::readCodewords() 
{
    codewords_t result = {0};
    
    auto formatInfo = readFormatInformation();
    const Version* version = readVersion();

    if (version == nullptr || !formatInfo)
    {
        return result;
    }

    // Get the data mask for the format used in this QR Code. This will exclude
    // some bits from reading as we wind through the bit matrix.
    auto dataMask = DataMask::select((int) formatInfo.value().getDataMask());
    if (!dataMask)
    {
        return result;
    }
       
    int dimension = bitMatrix_.getHeight();
    dataMask.value().unmaskBitMatrix(bitMatrix_, dimension);

    bitmatrix_t<config::MaxGridSize, config::MaxGridSize> functionPattern(dimension);
    version->buildFunctionPattern(functionPattern);
    
    bool readingUp = true;
    int resultOffset = 0;
    int currentByte = 0;
    int bitsRead = 0;
   
    // Read columns in pairs, from right to left
    for (int x = dimension - 1; x > 0; x -= 2) 
    {
        if (x == 6) 
        {
            // Skip whole column with vertical alignment pattern;
            // saves time and makes the other code proceed more cleanly
            x--;
        }
        // Read alternatingly from bottom to top then top to bottom
        for (int counter = 0; counter < dimension; counter++) 
        {
            int y = readingUp ? dimension - 1 - counter : counter;
            for (int col = 0; col < 2; col++) 
            {
                // Ignore bits covered by the function pattern
                if (!functionPattern.get(x - col, y)) 
                {
                    // Read a bit
                    bitsRead++;
                    currentByte <<= 1;
                    
                    if (bitMatrix_.get(x - col, y)) 
                    {
                        currentByte |= 1;
                    }
          
                    // If we've made a whole byte, save it off
                    if (bitsRead == 8) 
                    {
                        result[resultOffset++] = (char)currentByte;
                        bitsRead = 0;
                        currentByte = 0;
                    }
                }
            }
        }
    
        readingUp = !readingUp; // switch directions
    }

    if (resultOffset != version->getTotalCodewords()) 
    {
        for (auto n = 0; n < result.size(); ++n)
        {
            result[n] = 0;
        }
    }
    
    return result;
}

} // ns qrcode
} // ns zxing

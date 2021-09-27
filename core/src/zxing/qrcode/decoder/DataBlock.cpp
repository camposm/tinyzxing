/*
 *  DataBlock.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 19/05/2008.
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

#include <zxing/qrcode/decoder/DataBlock.h>


namespace zxing {
namespace qrcode {

std::optional<datablocks_t> DataBlock::getDataBlocks(
    const config::codewords_t& rawCodewords,
    const Version* version,
    int ecLevel) 
{
    // Decoder already checks for invalid ecLevel
    const ECBlocks& ecBlocks = version->getECBlocksForLevel(ecLevel);
    const auto& ecBlockArray = ecBlocks.getECBlocks();

    datablocks_t result;
    const auto actualResultSize = config::totalDataBlocksPerVersion[version->getVersionNumber() - 1];

    int numResultBlocks = 0;
    for (size_t j = 0; j < ecBlockArray.size(); j++) 
    {
        for (int i = 0; i < ecBlockArray[j].getCount(); i++) 
        {
            int numDataCodewords = ecBlockArray[j].getDataCodewords();
            int numBlockCodewords = ecBlocks.getECCodewords() + numDataCodewords;

            if (numBlockCodewords > zxing::config::some_number_of_codewords)
            {
                // lib parameters were not adequately chosen.. warn the system and fail
                return {};
            }
            
            result[numResultBlocks].numDataCodewords_ = numDataCodewords;
            result[numResultBlocks].numBlockCodewords_ = numBlockCodewords;
            numResultBlocks += 1;
        }
    }

    // All blocks have the same amount of data, except that the last n
    // (where n may be 0) have 1 more byte. Figure out where these start.
    int shorterBlocksTotalCodewords = result[0].numBlockCodewords_;
    int longerBlocksStartAt = actualResultSize - 1;

    while (longerBlocksStartAt >= 0) 
    {
        int numCodewords = result[longerBlocksStartAt].numBlockCodewords_;
        
        if (numCodewords == shorterBlocksTotalCodewords) 
        {
            break;
        }
    
        if (numCodewords != shorterBlocksTotalCodewords + 1) 
        {
            return {};
        }
        
        longerBlocksStartAt--;
    }
    
    longerBlocksStartAt++;

    int shorterBlocksNumDataCodewords = shorterBlocksTotalCodewords - ecBlocks.getECCodewords();
    // The last elements of result may be 1 element longer;
    // first fill out as many elements as all of them have
    int rawCodewordsOffset = 0;

    for (int i = 0; i < shorterBlocksNumDataCodewords; i++) 
    {
        for (int j = 0; j < numResultBlocks; j++) 
        {
            result[j].codewords_[i] = rawCodewords[rawCodewordsOffset++];
        }
    }
  
    // Fill out the last data block in the longer ones
    for (int j = longerBlocksStartAt; j < numResultBlocks; j++) 
    {
        result[j].codewords_[shorterBlocksNumDataCodewords] = rawCodewords[rawCodewordsOffset++];
    }
  
    // Now add in error correction blocks
    int max = result[0].numBlockCodewords_;
    
    for (int i = shorterBlocksNumDataCodewords; i < max; i++) 
    {
        for (int j = 0; j < numResultBlocks; j++) 
        {
            int iOffset = j < longerBlocksStartAt ? i : i + 1;
            result[j].codewords_[iOffset] = rawCodewords[rawCodewordsOffset++];
        }
    }

    if (rawCodewordsOffset != version->getTotalCodewords()) 
    {
        return {};
    }

    return result;
}

} // ns qrcode
} // ns zxing

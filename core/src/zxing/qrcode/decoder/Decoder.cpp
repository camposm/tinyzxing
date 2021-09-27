/*
 *  Decoder.cpp
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

#include <zxing/qrcode/decoder/Decoder.h>
#include <zxing/qrcode/decoder/BitMatrixParser.h>
#include <zxing/qrcode/ErrorCorrectionLevel.h>
#include <zxing/qrcode/Version.h>
#include <zxing/qrcode/decoder/DataBlock.h>
#include <zxing/qrcode/decoder/DecodedBitStreamParser.h>


using zxing::qrcode::Decoder;


Decoder::Decoder() :
    rsDecoder_(zxing::get_QR_Field()) 
{
    // blank
}

zxing::config::decoded_output_t Decoder::decode (sampled_grid_t& bits) 
{
    // Construct a parser and read version, error-correction level
    BitMatrixParser parser(bits);

    const Version* version = parser.readVersion();
    if (version == nullptr)
    {
        return {};
    }
    
    int ecLevel = parser.readFormatInformation()->getErrorCorrectionLevel();
    if (ecLevel < 0)
    {
        return {};
    }

    // Read codewords
    codewords_t codewords = parser.readCodewords();

    // Separate into data blocks
    auto tmp = DataBlock::getDataBlocks(codewords, version, ecLevel);
    if (!tmp)
    {
        //std::cout << "datablock failed\n";
        return {};
    }
    
    auto& dataBlocks = tmp.value();
    auto actualResultSize = config::totalDataBlocksPerVersion[version->getVersionNumber() - 1];

    // Count total number of data bytes
    int totalBytes = 0;
    for (size_t i = 0; i < actualResultSize; i++) 
    {
        totalBytes += dataBlocks[i].numDataCodewords_;
    }
    
    if (totalBytes > config::MaxDecodedOutput)
    {
        // lib was not configured properly; decoding failed
        //std::cout << "E: totalBytes > config::MaxDecodedOutput\n";
        return {};
    }
    
    
    std::array<char, config::MaxDecodedOutput> resultBytes = {0};
    int resultOffset = 0;

    int numDataCodewords_;
    int numBlockCodewords_;


    // Error-correct and copy data blocks together into a stream of bytes
    for (size_t j = 0; j < actualResultSize; j++) 
    {
        int numECCodewords = dataBlocks[j].numBlockCodewords_ - dataBlocks[j].numDataCodewords_;
        if (rsDecoder_.decode(dataBlocks[j].codewords_, dataBlocks[j].numBlockCodewords_, numECCodewords))
        {
            // correction failed
            //std::cout << "e: correction failed\n";
            return {};
        }
        
        for (int i = 0; i < dataBlocks[j].numDataCodewords_; i++) 
        {
            resultBytes[resultOffset++] = dataBlocks[j].codewords_[i];
        }
    }

    return DecodedBitStreamParser::decode(resultBytes, totalBytes, version, ecLevel);
}

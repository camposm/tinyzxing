#ifndef __VERSION_H__
#define __VERSION_H__

/*
 *  Version.h
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


#include <array>
#include <zxing/qrcode/ErrorCorrectionLevel.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/common/GridSampler.h>


namespace zxing {
namespace qrcode {


class ECB 
{
public:
    constexpr ECB() : 
        count_(0), 
        dataCodewords_(0) 
    {
        // blank
    }
    
    constexpr ECB (int count, int dataCodewords) :
        count_(count), 
        dataCodewords_(dataCodewords) 
    {
        // blank
    }
    
    constexpr int getCount() const            { return count_; }
    constexpr int getDataCodewords() const    { return dataCodewords_; }
    
private:
    int count_;
    int dataCodewords_;  
};

namespace {

// TODO: move this to config
constexpr uint32_t MaxAlignmentCenters = 7;

}

using align_center_array_t = std::array<int, MaxAlignmentCenters>;


class ECBlocks 
{
public:
    constexpr ECBlocks() : 
        ecBlocksSize_(0),
        ecCodewords_(0),
        ecBlocks_{}
    {
        // blank
    }
        
    constexpr ECBlocks (int ecCodewords, ECB&& ecBlocks) :
        ecBlocksSize_(1),
        ecCodewords_(ecCodewords),
        ecBlocks_{ecBlocks, ECB()}
    {
        // blank
    }
        
    constexpr ECBlocks(int ecCodewords, ECB&& ecBlocks1, ECB&& ecBlocks2) :
        ecBlocksSize_(2),
        ecCodewords_(ecCodewords),
        ecBlocks_{ecBlocks1, ecBlocks2}
    {
        // blank        
    }
    
    constexpr int getECCodewords() const
    {
        return ecCodewords_;
    }
    
    constexpr int getECBlocksSize() const
    {
        return ecBlocksSize_;
    }
    
    constexpr const std::array<ECB, 2>& getECBlocks() const
    {
        return ecBlocks_;
    }
    
private:
    int ecBlocksSize_;
    int ecCodewords_;
    std::array<ECB, 2> ecBlocks_;  
};


class Version
{
public:
    constexpr Version();
    constexpr Version (
        int versionNumber, 
        align_center_array_t&& alignmentPatternCenters, 
        std::array<ECBlocks, 4>&& ecBlocks
    );

    constexpr int getVersionNumber() const
    {
        return versionNumber_;
    }

    constexpr const align_center_array_t& getAlignmentPatternCenters() const
    {
        return alignmentPatternCenters_;
    }

    constexpr int getTotalCodewords() const
    {
        return totalCodewords_;
    }

    constexpr int getDimensionForVersion() const
    {
        return 17 + 4 * versionNumber_;
    }
    
    const ECBlocks& getECBlocksForLevel (int ecLevel) const;
    
    static const Version* getProvisionalVersionForDimension (int dimension);
    static const Version* getVersionForNumber (int versionNumber);
    static const Version* decodeVersionInformation (unsigned int versionBits);
    
    void buildFunctionPattern (bitmatrix_i& functionPattern) const;

private:
    int versionNumber_;
    align_center_array_t alignmentPatternCenters_;    
    std::array<ECBlocks, 4> ecBlocks_;
    int totalCodewords_;

};

} // ns qrcode
} // ns zxing

#endif // __VERSION_H__

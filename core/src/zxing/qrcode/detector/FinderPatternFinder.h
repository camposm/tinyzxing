#ifndef __FINDER_PATTERN_FINDER_H__
#define __FINDER_PATTERN_FINDER_H__

/*
 *  FinderPatternFinder.h
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
#include <zxing/ZXingConfig.h>
#include <zxing/qrcode/detector/FinderPattern.h>
#include <zxing/qrcode/detector/FinderPatternInfo.h>
#include <zxing/common/BitMatrix.h>


namespace zxing {
namespace qrcode {

class FinderPatternFinder 
{
public:
    FinderPatternFinder (const bitmatrix_i& image);
    
    FinderPatternInfo find();
    static float distance(const ResultPoint& p1, const ResultPoint& p2);

protected:
    static const int MIN_SKIP;
    static const int MAX_MODULES;

    const bitmatrix_i& image_;
    std::array<FinderPattern, config::MaxPossibleCenters> possibleCenters_;
    uint32_t possibleCentersCount_;
    bool hasSkipped_;

    // stateCount must be int[5]
    static float centerFromEnd (int* stateCount, int end);
    static bool foundPatternCross (int* stateCount);

    float crossCheckVertical (size_t startI, size_t centerJ, int maxCount, int originalStateCountTotal);
    float crossCheckHorizontal (size_t startJ, size_t centerI, int maxCount, int originalStateCountTotal);

    // stateCount must be int[5]
    bool handlePossibleCenter (int* stateCount, size_t i, size_t j);
    int findRowSkip();
    bool haveMultiplyConfirmedCenters();
    
    std::array<FinderPattern, 3> selectBestPatterns();
    static void orderBestPatterns(std::array<FinderPattern, 3>& patterns);
  
private:
    static const int CENTER_QUORUM;  

};

} // ns qrcode
} // ns zxing

#endif // __FINDER_PATTERN_FINDER_H__

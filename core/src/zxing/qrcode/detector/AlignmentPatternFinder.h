#ifndef __ALIGNMENT_PATTERN_FINDER_H__
#define __ALIGNMENT_PATTERN_FINDER_H__

/*
 *  AlignmentPatternFinder.h
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

#include <zxing/qrcode/detector/AlignmentPattern.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/ZXingConfig.h>
#include <array>
#include <optional>


namespace zxing {
namespace qrcode {

using statecnt_t = std::array<int, 3>;

class AlignmentPatternFinder
{
public:
    AlignmentPatternFinder (const bitmatrix_i& image, int startX, int startY, int width, int height, float moduleSize);

    std::optional<AlignmentPattern> find();
  
private:
    static int CENTER_QUORUM;
    static int MIN_SKIP;
    static int MAX_MODULES;

    const bitmatrix_i& image_;
    std::array<AlignmentPattern, config::MaxPossibleCenters> possibleCenters_;
    uint32_t numberOfPossibleCenters_;
    
    int startX_;
    int startY_;
    int width_;
    int height_;
    float moduleSize_;

    static float centerFromEnd (const statecnt_t& stateCount, int end);
    bool foundPatternCross (const statecnt_t& stateCount);

    float crossCheckVertical(int startI, int centerJ, int maxCount, int originalStateCountTotal);

    std::optional<AlignmentPattern> handlePossibleCenter (const statecnt_t& stateCount, int i, int j);
};

} // ns qrcode
} // ns zxing

#endif // __ALIGNMENT_PATTERN_FINDER_H__

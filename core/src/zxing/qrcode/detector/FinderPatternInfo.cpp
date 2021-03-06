/*
 *  FinderPatternInfo.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 13/05/2008.
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

#include <zxing/qrcode/detector/FinderPatternInfo.h>

namespace zxing {
namespace qrcode {

FinderPatternInfo::FinderPatternInfo (const std::array<FinderPattern, 3>& patternCenters) :
    bottomLeft_(patternCenters[0]), 
    topLeft_(patternCenters[1]), 
    topRight_(patternCenters[2]) 
{
    // blank
}

const FinderPattern& FinderPatternInfo::getBottomLeft() const
{
    return bottomLeft_;
}

const FinderPattern& FinderPatternInfo::getTopLeft() const
{
    return topLeft_;
}

const FinderPattern& FinderPatternInfo::getTopRight() const
{
    return topRight_;
}

} // ns qrcode
} // ns zxing

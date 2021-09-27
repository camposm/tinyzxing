#ifndef __FINDER_PATTERN_INFO_H__
#define __FINDER_PATTERN_INFO_H__

/*
 *  FinderPatternInfo.h
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

#include <zxing/qrcode/detector/FinderPattern.h>
#include <array>

namespace zxing {
namespace qrcode {

class FinderPatternInfo
{
public:
    FinderPatternInfo (const std::array<FinderPattern, 3>& patternCenters);

    const FinderPattern& getBottomLeft() const;
    const FinderPattern& getTopLeft() const;
    const FinderPattern& getTopRight() const;

private:
    FinderPattern bottomLeft_;
    FinderPattern topLeft_;
    FinderPattern topRight_;
};

} // ns qrcode
} // ns zxing

#endif // __FINDER_PATTERN_INFO_H__

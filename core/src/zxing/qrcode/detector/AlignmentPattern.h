#ifndef __ALIGNMENT_PATTERN_H__
#define __ALIGNMENT_PATTERN_H__

/*
 *  AlignmentPattern.h
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

#include <zxing/ResultPoint.h>


namespace zxing {
namespace qrcode {

class AlignmentPattern : public ResultPoint 
{
public:
    AlignmentPattern();
    AlignmentPattern(float posX, float posY, float estimatedModuleSize);

    void set (float new_posX, float new_posY, float new_estimatedModuleSize);
        
    float getEstimatedModuleSize() const { return estimatedModuleSize_; }

    bool aboutEquals (float moduleSize, float i, float j) const;
    AlignmentPattern combineEstimate(float i, float j, float newModuleSize) const;

private:
    float estimatedModuleSize_;
    
};

} // ns qrcode
} // ns zxing

#endif // __ALIGNMENT_PATTERN_H__

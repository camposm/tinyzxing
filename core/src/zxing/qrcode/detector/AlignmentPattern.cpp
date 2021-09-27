/*
 *  AlignmentPattern.cpp
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

#include <cmath>
#include <zxing/qrcode/detector/AlignmentPattern.h>


using zxing::qrcode::AlignmentPattern;

AlignmentPattern::AlignmentPattern() :
    ResultPoint(-1.0f, -1.0f), 
    estimatedModuleSize_(-1.0f) 
{
    // blank
}

AlignmentPattern::AlignmentPattern (float posX, float posY, float estimatedModuleSize) :
    ResultPoint(posX, posY), 
    estimatedModuleSize_(estimatedModuleSize) 
{
    // blank
}

void AlignmentPattern::set (float new_posX, float new_posY, float new_estimatedModuleSize)
{
    this->first = new_posX;
    this->second = new_posY;
    estimatedModuleSize_ = new_estimatedModuleSize;
}

bool AlignmentPattern::aboutEquals (float moduleSize, float i, float j) const 
{
    if (std::abs(i - this->second) <= moduleSize && std::abs(j - this->first) <= moduleSize) 
    {
        float moduleSizeDiff = std::abs(moduleSize - estimatedModuleSize_);
        return moduleSizeDiff <= 1.0f || moduleSizeDiff <= estimatedModuleSize_;
    }
    
    return false;
}

AlignmentPattern AlignmentPattern::combineEstimate (float i, float j, float newModuleSize) const 
{
    float combinedX = (this->first + j) / 2.0f;
    float combinedY = (this->second + i) / 2.0f;
    float combinedModuleSize = (estimatedModuleSize_ + newModuleSize) / 2.0f;
    
    return AlignmentPattern(combinedX, combinedY, combinedModuleSize);
}

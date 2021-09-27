/*
 *  ResultPoint.cpp
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

#include <zxing/ResultPoint.h>
#include <zxing/common/detector/MathUtils.h>

using zxing::common::detector::MathUtils;

namespace zxing {
namespace {

float crossProductZ (const ResultPoint& pointA, const ResultPoint& pointB, const ResultPoint& pointC) 
{
    float bX = pointB.first;
    float bY = pointB.second;
    return ((pointC.first - bX) * (pointA.second - bY)) - ((pointC.second - bY) * (pointA.first - bX));
}

} // ns -anon-

void resultpoint::orderBestPatterns (std::array<ResultPoint, 3>& patterns)
{
    // Find distances between pattern centers
    float zeroOneDistance = distance(
        patterns[0].first, patterns[1].first, 
        patterns[0].second, patterns[1].second
    );
    float oneTwoDistance = distance(
        patterns[1].first, patterns[2].first,
        patterns[1].second, patterns[2].second
    );
    float zeroTwoDistance = distance(
        patterns[0].first, patterns[2].first,
        patterns[0].second, patterns[2].second
    );

    ResultPoint pointA, pointB, pointC;
    
    // Assume one closest to other two is B; A and C will just be guesses at first
    if (oneTwoDistance >= zeroOneDistance && oneTwoDistance >= zeroTwoDistance) 
    {
        pointB = patterns[0];
        pointA = patterns[1];
        pointC = patterns[2];
    } 
    else if (zeroTwoDistance >= oneTwoDistance && zeroTwoDistance >= zeroOneDistance) 
    {
        pointB = patterns[1];
        pointA = patterns[0];
        pointC = patterns[2];
    } 
    else 
    {
        pointB = patterns[2];
        pointA = patterns[0];
        pointC = patterns[1];
    }

    // Use cross product to figure out whether A and C are correct or flipped.
    // This asks whether BC x BA has a positive z component, which is the arrangement
    // we want for A, B, C. If it's negative, then we've got it flipped around and
    // should swap A and C.
    if (crossProductZ(pointA, pointB, pointC) < 0.0f) 
    {
        ResultPoint temp(pointA);
        pointA = pointC;
        pointC = temp;
    }

    patterns[0] = pointA;
    patterns[1] = pointB;
    patterns[2] = pointC;
}

float resultpoint::distance(const ResultPoint& pattern1, const ResultPoint& pattern2) 
{
    return MathUtils::distance(
        pattern1.first,
        pattern1.second,
        pattern2.first,
        pattern2.second
     );
}

float resultpoint::distance(float x1, float x2, float y1, float y2) 
{
    float xDiff = x1 - x2;
    float yDiff = y1 - y2;
    return (float) sqrt((double) (xDiff * xDiff + yDiff * yDiff));
}

} // ns zxing

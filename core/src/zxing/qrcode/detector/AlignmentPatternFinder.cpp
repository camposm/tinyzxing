/*
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

#include <limits>
#include <array>
#include <cmath>
#include <cstdlib>
#include <zxing/qrcode/detector/AlignmentPatternFinder.h>
#include <zxing/ZXing.h>


using zxing::qrcode::AlignmentPatternFinder;
using zxing::qrcode::AlignmentPattern;


float AlignmentPatternFinder::centerFromEnd (const statecnt_t& stateCount, int end) 
{
    return (float)(end - stateCount[2]) - stateCount[1] / 2.0f;
}

bool AlignmentPatternFinder::foundPatternCross (const statecnt_t& stateCount) 
{
    float maxVariance = moduleSize_ / 2.0f;
    
    for (int i = 0; i < 3; i++) 
    {
        if (std::abs(moduleSize_ - stateCount[i]) >= maxVariance) 
        {
            return false;
        }
    }
    
    return true;
}

float AlignmentPatternFinder::crossCheckVertical (int startI, int centerJ, int maxCount, int originalStateCountTotal) 
{
    int maxI = image_.getHeight();
    statecnt_t stateCount = {0};

    // Start counting up from center
    int i = startI;
    while (i >= 0 && image_.get(centerJ, i) && stateCount[1] <= maxCount) 
    {
        stateCount[1]++;
        i--;
    }
    
    // If already too many modules in this state or ran off the edge:
    if (i < 0 || stateCount[1] > maxCount) 
    {
        return zxing::nan();
    }
    
    while (i >= 0 && !image_.get(centerJ, i) && stateCount[0] <= maxCount) 
    {
        stateCount[0]++;
        i--;
    }
    
    if (stateCount[0] > maxCount) 
    {
        return zxing::nan();
    }

    // Now also count down from center
    i = startI + 1;
    while (i < maxI && image_.get(centerJ, i) && stateCount[1] <= maxCount) 
    {
        stateCount[1]++;
        i++;
    }
    
    if (i == maxI || stateCount[1] > maxCount) 
    {
        return zxing::nan();
    }
    
    while (i < maxI && !image_.get(centerJ, i) && stateCount[2] <= maxCount) 
    {
        stateCount[2]++;
        i++;
    }
    
    if (stateCount[2] > maxCount) 
    {
        return zxing::nan();
    }

    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
    if (5 * abs(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal) 
    {
        return zxing::nan();
    }

    return foundPatternCross(stateCount) ? centerFromEnd(stateCount, i) : zxing::nan();
}

std::optional<AlignmentPattern> AlignmentPatternFinder::handlePossibleCenter (const statecnt_t& stateCount, int i, int j) 
{
    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
    float centerJ = centerFromEnd(stateCount, j);
    float centerI = crossCheckVertical(i, (int)centerJ, 2 * stateCount[1], stateCountTotal);
  
    if (!isnan(centerI)) 
    {
        float estimatedModuleSize = (float)(stateCount[0] + stateCount[1] + stateCount[2]) / 3.0f;
        int max = numberOfPossibleCenters_;
    
        for (int index = 0; index < max; index++) 
        {
            const AlignmentPattern& center = possibleCenters_[index];
            // Look for about the same center and module size:
            if (center.aboutEquals(estimatedModuleSize, centerI, centerJ)) 
            {
                return center.combineEstimate(centerI, centerJ, estimatedModuleSize);
            }
        }
        
        if (numberOfPossibleCenters_ < config::MaxPossibleCenters)
        {
            possibleCenters_[numberOfPossibleCenters_].set(centerJ, centerI, estimatedModuleSize);
            ++numberOfPossibleCenters_;
        }
    }
  
    return {};
}

AlignmentPatternFinder::AlignmentPatternFinder(
    const bitmatrix_i& image, 
    int startX, int startY, int width, int height, 
    float moduleSize) :
    image_(image), 
    possibleCenters_{}, 
    numberOfPossibleCenters_(0),
    startX_(startX), startY_(startY),
    width_(width), height_(height), 
    moduleSize_(moduleSize) 
{
    // blank
}

std::optional<AlignmentPattern> AlignmentPatternFinder::find() 
{
    int maxJ = startX_ + width_;
    int middleI = startY_ + (height_ >> 1);

    // We are looking for black/white/black modules in 1:1:1 ratio;
    // this tracks the number of black/white/black modules seen so far
    statecnt_t stateCount = {0};

    for (int iGen = 0; iGen < height_; iGen++) 
    {
        // Search from middle outwards
        int i = middleI + ((iGen & 0x01) == 0 ? ((iGen + 1) >> 1) : -((iGen + 1) >> 1));

        stateCount[0] = 0;
        stateCount[1] = 0;
        stateCount[2] = 0;
        
        int j = startX_;
        // Burn off leading white pixels before anything else; if we start in the middle of
        // a white run, it doesn't make sense to count its length, since we don't know if the
        // white run continued to the left of the start point
        while (j < maxJ && !image_.get(j, i)) 
        {
            j++;
        }
        
        int currentState = 0;
        while (j < maxJ) 
        {
            if (image_.get(j, i)) 
            {
                // Black pixel
                if (currentState == 1) 
                { 
                    // Counting black pixels
                    stateCount[currentState]++;
                } 
                else 
                { 
                    // Counting white pixels
                    if (currentState == 2) 
                    { 
                        // A winner?
                        if (foundPatternCross(stateCount)) 
                        { 
                            // Yes
                            auto confirmed = handlePossibleCenter(stateCount, i, j);
                            if (confirmed) 
                            {
                                return confirmed;
                            }
                        }
                        
                        stateCount[0] = stateCount[2];
                        stateCount[1] = 1;
                        stateCount[2] = 0;
                        currentState = 1;
                    } 
                    else 
                    {
                        stateCount[++currentState]++;
                    }
                }
            } 
            else 
            { 
                // White pixel
                if (currentState == 1) 
                { 
                    // Counting black pixels
                    currentState++;
                }
                
                stateCount[currentState]++;
            }
            
            j++;
        }

        if (foundPatternCross(stateCount)) 
        {
            auto confirmed = handlePossibleCenter(stateCount, i, maxJ);
            if (confirmed) 
            {
                return confirmed;
            }
        }
    }

    // Hmm, nothing we saw was observed and confirmed twice. If we had
    // any guess at all, return it.
    if (numberOfPossibleCenters_ > 0) 
    {
        return possibleCenters_[0];
    }

    return {};
}

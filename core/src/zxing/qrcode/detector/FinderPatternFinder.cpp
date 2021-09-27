/*
 *  FinderPatternFinder.cpp
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

#include <iostream>
#include <algorithm>
#include <zxing/qrcode/detector/FinderPatternFinder.h>
#include <zxing/ZXingConfig.h>
#include <zxing/ZXing.h>


using zxing::qrcode::FinderPatternFinder;
using zxing::qrcode::FinderPattern;
using zxing::qrcode::FinderPatternInfo;

// VC++
using zxing::bitmatrix_i;
using zxing::ResultPoint;


namespace {

class FurthestFromAverageComparator 
{
public:
    FurthestFromAverageComparator (float averageModuleSize) :
        averageModuleSize_(averageModuleSize) 
    {
        // blank
    }
    
    bool operator() (const FinderPattern& a, const FinderPattern& b) 
    {
        float dA = std::abs(a.getEstimatedModuleSize() - averageModuleSize_);
        float dB = std::abs(b.getEstimatedModuleSize() - averageModuleSize_);
        return dA > dB;
    }
  
private:
    const float averageModuleSize_;  
};

class CenterComparator 
{
public:
    CenterComparator (float averageModuleSize) :
        averageModuleSize_(averageModuleSize) 
    {
        // blank
    }
    
    bool operator()(const FinderPattern& a, const FinderPattern& b) 
    {
        // N.B.: we want the result in descending order ...
        if (a.getCount() != b.getCount()) 
        {
            return a.getCount() > b.getCount();
        } 
        else 
        {
            float dA = std::abs(a.getEstimatedModuleSize() - averageModuleSize_);
            float dB = std::abs(b.getEstimatedModuleSize() - averageModuleSize_);
            return dA < dB;
        }
    }
  
private:
    const float averageModuleSize_;    
};

}

const int FinderPatternFinder::CENTER_QUORUM = 2;
const int FinderPatternFinder::MIN_SKIP = 3;
const int FinderPatternFinder::MAX_MODULES = 57;

float FinderPatternFinder::centerFromEnd (int* stateCount, int end) 
{
    return (float)(end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0f;
}

bool FinderPatternFinder::foundPatternCross (int* stateCount) 
{
    int totalModuleSize = 0;
    
    for (int i = 0; i < 5; i++) 
    {
        if (stateCount[i] == 0) 
        {
            return false;
        }
        totalModuleSize += stateCount[i];
    }
    
    if (totalModuleSize < 7) 
    {
        return false;
    }
    
    float moduleSize = (float)totalModuleSize / 7.0f;
    float maxVariance = moduleSize / 2.0f;
    // Allow less than 50% variance from 1-1-3-1-1 proportions
    return std::abs(moduleSize - stateCount[0]) < maxVariance && 
            std::abs(moduleSize - stateCount[1]) < maxVariance && 
            std::abs(3.0f * moduleSize - stateCount[2]) < (3.0f * maxVariance) && 
            std::abs(moduleSize - stateCount[3]) < maxVariance && 
            std::abs(moduleSize - stateCount[4]) < maxVariance;
}

float FinderPatternFinder::crossCheckVertical (size_t startI, size_t centerJ, int maxCount, int originalStateCountTotal) 
{
    int maxI = image_.getHeight();
    int stateCount[5];

    for (int i = 0; i < 5; i++)
    {
        stateCount[i] = 0;
    }

    // Start counting up from center
    int i = startI;
    while (i >= 0 && image_.get(centerJ, i)) 
    {
        stateCount[2]++;
        i--;
    }
    
    if (i < 0) 
    {
        return zxing::nan();
    }
    
    while (i >= 0 && !image_.get(centerJ, i) && stateCount[1] <= maxCount) 
    {
        stateCount[1]++;
        i--;
    }
    
    // If already too many modules in this state or ran off the edge:
    if (i < 0 || stateCount[1] > maxCount) 
    {
        return zxing::nan();
    }
    
    while (i >= 0 && image_.get(centerJ, i) && stateCount[0] <= maxCount) 
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
    while (i < maxI && image_.get(centerJ, i))
    {
        stateCount[2]++;
        i++;
    }
  
    if (i == maxI) 
    {
        return zxing::nan();
    }
    
    while (i < maxI && !image_.get(centerJ, i) && stateCount[3] < maxCount) 
    {
        stateCount[3]++;
        i++;
    }
    
    if (i == maxI || stateCount[3] >= maxCount) 
    {
        return zxing::nan();
    }
  
    while (i < maxI && image_.get(centerJ, i) && stateCount[4] < maxCount) 
    {
        stateCount[4]++;
        i++;
    }
  
    if (stateCount[4] >= maxCount) 
    {
        return zxing::nan();
    }

    // If we found a finder-pattern-like section, but its size is more than 40% different than
    // the original, assume it's a false positive
    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
    if (5 * std::abs(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal) 
    {
        return zxing::nan();
    }

    return foundPatternCross(stateCount) ? centerFromEnd(stateCount, i) : zxing::nan();
}

float FinderPatternFinder::crossCheckHorizontal (size_t startJ, size_t centerI, int maxCount, int originalStateCountTotal) 
{
    int maxJ = image_.getWidth();
    int stateCount[5];
    
    for (int i = 0; i < 5; i++)
    {
        stateCount[i] = 0;
    }

    int j = startJ;
    while (j >= 0 && image_.get(j, centerI)) 
    {
        stateCount[2]++;
        j--;
    }
  
    if (j < 0) 
    {
        return zxing::nan();
    }
    while (j >= 0 && !image_.get(j, centerI) && stateCount[1] <= maxCount) 
    {
        stateCount[1]++;
        j--;
    }
    
    if (j < 0 || stateCount[1] > maxCount) 
    {
        return zxing::nan();
    }
  
    while (j >= 0 && image_.get(j, centerI) && stateCount[0] <= maxCount) 
    {
        stateCount[0]++;
        j--;
    }
  
    if (stateCount[0] > maxCount) 
    {
        return zxing::nan();
    }

    j = startJ + 1;
    while (j < maxJ && image_.get(j, centerI)) 
    {
        stateCount[2]++;
        j++;
    }
  
    if (j == maxJ) 
    {
        return zxing::nan();
    }
  
    while (j < maxJ && !image_.get(j, centerI) && stateCount[3] < maxCount) 
    {
        stateCount[3]++;
        j++;
    }
  
    if (j == maxJ || stateCount[3] >= maxCount) 
    {
        return zxing::nan();
    }
    
    while (j < maxJ && image_.get(j, centerI) && stateCount[4] < maxCount) 
    {
        stateCount[4]++;
        j++;
    }
  
    if (stateCount[4] >= maxCount) 
    {
        return zxing::nan();
    }

    // If we found a finder-pattern-like section, but its size is significantly different than
    // the original, assume it's a false positive
    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];

    if (5 * std::abs(stateCountTotal - originalStateCountTotal) >= originalStateCountTotal) 
    {
        return zxing::nan();
    }

    return foundPatternCross(stateCount) ? centerFromEnd(stateCount, j) : zxing::nan();
}

bool FinderPatternFinder::handlePossibleCenter (int* stateCount, size_t i, size_t j) 
{
    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
    float centerJ = centerFromEnd(stateCount, j);
    float centerI = crossCheckVertical(i, (size_t)centerJ, stateCount[2], stateCountTotal);
  
    if (!isnan(centerI)) 
    {
        // Re-cross check
        centerJ = crossCheckHorizontal((size_t)centerJ, (size_t)centerI, stateCount[2], stateCountTotal);
        if (!isnan(centerJ)) 
        {
            float estimatedModuleSize = (float)stateCountTotal / 7.0f;
            bool found = false;

            //std::cout << "FinderPatternFinder::possibleCenters_.size() = " << possibleCentersCount_ << "\n";
            for (size_t index = 0; index < possibleCentersCount_; index++) 
            {
                FinderPattern center = possibleCenters_[index];
                // Look for about the same center and module size:
                if (center.aboutEquals(estimatedModuleSize, centerI, centerJ)) 
                {
                    possibleCenters_[index] = center.combineEstimate(centerI, centerJ, estimatedModuleSize);
                    found = true;
                    break;
                }
            }
      
            if (!found) 
            {
                if (possibleCentersCount_ < config::MaxPossibleCenters)
                {
                    possibleCenters_[possibleCentersCount_] = FinderPattern(centerJ, centerI, estimatedModuleSize);
                    ++possibleCentersCount_;
                }
                else
                {
                    // wtf
                    //std::cout << "w: missed candidate??\n";
                }
            }
            
            return true;
        }
    }
  
    return false;
}

int FinderPatternFinder::findRowSkip() 
{
    if (possibleCentersCount_ <= 1) 
    {
        return 0;
    }
    
    FinderPattern firstConfirmedCenter;
    bool hasFirstConfirmedCenter = false;
    
    for (size_t i = 0; i < possibleCentersCount_; i++) 
    {
        const FinderPattern& center = possibleCenters_[i];
        if (center.getCount() >= CENTER_QUORUM) 
        {
            if (hasFirstConfirmedCenter)
            {
                // We have two confirmed centers
                // How far down can we skip before resuming looking for the next
                // pattern? In the worst case, only the difference between the
                // difference in the x / y coordinates of the two centers.
                // This is the case where you find top left first. Draw it out.
                hasSkipped_ = true;
                return (int)(std::abs(firstConfirmedCenter.first - center.first) - std::abs(firstConfirmedCenter.second - center.second))/2;
            }
            else
            {
                firstConfirmedCenter = center;
                hasFirstConfirmedCenter = true;
            }
        }
    }
  
    return 0;
}

bool FinderPatternFinder::haveMultiplyConfirmedCenters() 
{
    int confirmedCount = 0;
    float totalModuleSize = 0.0f;

    for (size_t i = 0; i < possibleCentersCount_; i++) 
    {
        const FinderPattern& pattern = possibleCenters_[i];
        if (pattern.getCount() >= CENTER_QUORUM) 
        {
            confirmedCount++;
            totalModuleSize += pattern.getEstimatedModuleSize();
        }
    }
  
    if (confirmedCount < 3) 
    {
        return false;
    }

    // OK, we have at least 3 confirmed centers, but, it's possible that one is a "false positive"
    // and that we need to keep looking. We detect this by asking if the estimated module sizes
    // vary too much. We arbitrarily say that when the total deviation from average exceeds
    // 5% of the total module size estimates, it's too much.
    float average = totalModuleSize / possibleCentersCount_;
    float totalDeviation = 0.0f;
    for (size_t i = 0; i < possibleCentersCount_; i++) 
    {
        const FinderPattern& pattern = possibleCenters_[i];
        totalDeviation += abs(pattern.getEstimatedModuleSize() - average);
    }
    
    return totalDeviation <= 0.05f * totalModuleSize;
}

std::array<FinderPattern, 3> FinderPatternFinder::selectBestPatterns() 
{
    if (possibleCentersCount_ < 3) 
    {
        return {};
    }

    //std::cout << "possibleCenters_.size = " << possibleCentersCount_ << "\n";

/*
    TODO: understand how to filter the centers

    // Filter outlier possibilities whose module size is too different
    if (startSize > 3) 
    {
        // But we can only afford to do so if we have at least 4 possibilities to choose from
        float totalModuleSize = 0.0f;
        float square = 0.0f;

        for (size_t i = 0; i < startSize; i++) 
        {
            float size = possibleCenters_[i].getEstimatedModuleSize();
            totalModuleSize += size;
            square += size * size;
        }
        
        float average = totalModuleSize / (float) startSize;
        float stdDev = (float)sqrt(square / startSize - average * average);



        for (int zzz = 0; zzz < possibleCentersCount_; ++zzz)
        {
            const auto& foo = possibleCenters_[zzz];
            std::cout << "[" << zzz << "] " << foo.first << " " << foo.second << " thisavg=";
            std::cout << foo.getEstimatedModuleSize() << " avg=" << average << "\n";
        }
        std::cout << "------------------------------------------\n";

        std::sort(possibleCenters_.begin(), possibleCenters_.begin() + possibleCentersCount_, FurthestFromAverageComparator(average));

        for (int zzz = 0; zzz < possibleCentersCount_; ++zzz)
        {
            const auto& foo = possibleCenters_[zzz];
            std::cout << "[" << zzz << "] " << foo.first << " " << foo.second << " thisavg=";
            std::cout << foo.getEstimatedModuleSize() << " avg=" << average << "\n";
        }
        std::cout << "------------------------------------------\n";

        float limit = std::max(0.2f * average, stdDev);

        std::cout << "limit=" << limit << "\n";

        for (size_t i = 0; i < possibleCenters_.size() && possibleCenters_.size() > 3; i++) 
        {
            if (abs(possibleCenters_[i].getEstimatedModuleSize() - average) > limit) 
            {
                //possibleCenters_.erase(possibleCenters_.begin()+i);
                i--;
            }
        }
    }
*/

    if (possibleCentersCount_ > 3) 
    {
        // Throw away all but those first size candidate points we found.
        float totalModuleSize = 0.0f;
        for (uint32_t i = 0; i < possibleCentersCount_; i++) 
        {
            float size = possibleCenters_[i].getEstimatedModuleSize();
            totalModuleSize += size;
        }
        
        float average = totalModuleSize / (float) possibleCentersCount_;
        std::sort(possibleCenters_.begin(), possibleCenters_.begin() + possibleCentersCount_, CenterComparator(average));
    }

    if (possibleCentersCount_ > 3) 
    {
        possibleCentersCount_ = 3;
        //possibleCenters_.erase(possibleCenters_.begin()+3,possibleCenters_.end());
    }

    return {possibleCenters_[0], possibleCenters_[1], possibleCenters_[2]};
}

void FinderPatternFinder::orderBestPatterns (std::array<FinderPattern, 3>& patterns) 
{
    // Find distances between pattern centers
    float abDistance = distance(patterns[0], patterns[1]);
    float bcDistance = distance(patterns[1], patterns[2]);
    float acDistance = distance(patterns[0], patterns[2]);

    FinderPattern topLeft;
    FinderPattern topRight;
    FinderPattern bottomLeft;

    // Assume one closest to other two is top left;
    // topRight and bottomLeft will just be guesses below at first
    if (bcDistance >= abDistance && bcDistance >= acDistance) 
    {
        topLeft = patterns[0];
        topRight = patterns[1];
        bottomLeft = patterns[2];
    } 
    else if (acDistance >= bcDistance && acDistance >= abDistance) 
    {
        topLeft = patterns[1];
        topRight = patterns[0];
        bottomLeft = patterns[2];
    } 
    else 
    {
        topLeft = patterns[2];
        topRight = patterns[0];
        bottomLeft = patterns[1];
    }

    // Use cross product to figure out which of other1/2 is the bottom left
    // pattern. The vector "top-left -> bottom-left" x "top-left -> top-right"
    // should yield a vector with positive z component
    if ((bottomLeft.second - topLeft.second) * (topRight.first - topLeft.first) < 
        (bottomLeft.first - topLeft.first) * (topRight.second - topLeft.second)) 
    {
        FinderPattern temp = topRight;
        topRight = bottomLeft;
        bottomLeft = temp;
    }

    patterns = {bottomLeft, topLeft, topRight};
}

float FinderPatternFinder::distance(const ResultPoint& p1, const ResultPoint& p2) 
{
    float dx = p1.first - p2.first;
    float dy = p1.second - p2.second;
    return (float)sqrt(dx * dx + dy * dy);
}

FinderPatternFinder::FinderPatternFinder (const bitmatrix_i& image) :
    image_(image), 
    possibleCenters_{},
    possibleCentersCount_(0),
    hasSkipped_(false) 
{
    // blank
}

FinderPatternInfo FinderPatternFinder::find() 
{
    size_t maxI = image_.getHeight();
    size_t maxJ = image_.getWidth();

    // We are looking for black/white/black/white/black modules in
    // 1:1:3:1:1 ratio; this tracks the number of such modules seen so far

    // As this is used often, we use an integer array instead of vector
    int stateCount[5];
    bool done = false;


    // Let's assume that the maximum version QR Code we support takes up 1/4
    // the height of the image, and then account for the center being 3
    // modules in size. This gives the smallest number of pixels the center
    // could be, so skip this often. When trying harder, look for all
    // QR versions regardless of how dense they are.
    int iSkip = (3 * maxI) / (4 * MAX_MODULES);
    if (iSkip < MIN_SKIP) 
    {
        iSkip = MIN_SKIP;
    }

    for (size_t i = iSkip - 1; i < maxI && !done; i += iSkip) 
    {
        // Get a row of black/white values
        stateCount[0] = 0;
        stateCount[1] = 0;
        stateCount[2] = 0;
        stateCount[3] = 0;
        stateCount[4] = 0;
        int currentState = 0;
        
        for (size_t j = 0; j < maxJ; j++) 
        {
            if (image_.get(j, i)) 
            {
                // Black pixel
                if ((currentState & 1) == 1) 
                {
                    // Counting white pixels
                    currentState++;
                }
                stateCount[currentState]++;
            } 
            else 
            { 
                // White pixel
                if ((currentState & 1) == 0) 
                { 
                    // Counting black pixels
                    if (currentState == 4) 
                    { 
                        // A winner?
                        if (foundPatternCross(stateCount)) 
                        {
                            // Yes
                            bool confirmed = handlePossibleCenter(stateCount, i, j);
                            if (confirmed) 
                            {
                                // Start examining every other line. Checking each line turned out to be too
                                // expensive and didn't improve performance.
                                iSkip = 2;
                                if (hasSkipped_) 
                                {
                                    done = haveMultiplyConfirmedCenters();
                                } 
                                else 
                                {
                                    int rowSkip = findRowSkip();
                                    if (rowSkip > stateCount[2]) 
                                    {
                                        // Skip rows between row of lower confirmed center
                                        // and top of presumed third confirmed center
                                        // but back up a bit to get a full chance of detecting
                                        // it, entire width of center of finder pattern

                                        // Skip by rowSkip, but back off by stateCount[2] (size
                                        // of last center of pattern we saw) to be conservative,
                                        // and also back off by iSkip which is about to be
                                        // re-added
                                        i += rowSkip - stateCount[2] - iSkip;
                                        j = maxJ - 1;
                                    }
                                }
                            } 
                            else 
                            {
                                stateCount[0] = stateCount[2];
                                stateCount[1] = stateCount[3];
                                stateCount[2] = stateCount[4];
                                stateCount[3] = 1;
                                stateCount[4] = 0;
                                currentState = 3;
                                continue;
                            }
                            // Clear state to start looking again
                            currentState = 0;
                            stateCount[0] = 0;
                            stateCount[1] = 0;
                            stateCount[2] = 0;
                            stateCount[3] = 0;
                            stateCount[4] = 0;
                        } 
                        else 
                        { 
                            // No, shift counts back by two
                            stateCount[0] = stateCount[2];
                            stateCount[1] = stateCount[3];
                            stateCount[2] = stateCount[4];
                            stateCount[3] = 1;
                            stateCount[4] = 0;
                            currentState = 3;
                        }
                    } 
                    else 
                    {
                        stateCount[++currentState]++;
                    }
                } 
                else 
                { 
                    // Counting white pixels
                    stateCount[currentState]++;
                }
            }
        }
        if (foundPatternCross(stateCount)) 
        {
            bool confirmed = handlePossibleCenter(stateCount, i, maxJ);
            if (confirmed) 
            {
                iSkip = stateCount[0];
                if (hasSkipped_) 
                {
                  // Found a third one
                  done = haveMultiplyConfirmedCenters();
                }
            }
        }
    }

    auto patternInfo = selectBestPatterns();
    orderBestPatterns(patternInfo);

    return FinderPatternInfo(patternInfo);
}

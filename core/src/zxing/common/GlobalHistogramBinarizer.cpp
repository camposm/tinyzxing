/*
 *  GlobalHistogramBinarizer.cpp
 *  zxing
 *
 *  Copyright 2010 ZXing authors. All rights reserved.
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
#include <zxing/common/GlobalHistogramBinarizer.h>


using zxing::GlobalHistogramBinarizer;
using zxing::Binarizer;
using zxing::bitmatrix_i;


GlobalHistogramBinarizer::GlobalHistogramBinarizer (LuminanceSource& source) : 
    Binarizer(source),
    buckets{0}
{
    // blank
}

const bitmatrix_i& GlobalHistogramBinarizer::getBlackMatrix() 
{
    LuminanceSource& source = getLuminanceSource();
    
    constexpr auto width = zxing::config::ImageWidth;
    constexpr auto height = zxing::config::ImageHeight;

    // Quickly calculates the histogram by sampling four rows from the image.
    // This proved to be more robust on the blackbox tests than sampling a
    // diagonal as we used to do.
    for (auto n = 0; n < LUMINANCE_BUCKETS; ++n)
    {
        buckets[n] = 0;
    }
    matrix.clear();
    
    for (int y = 1; y < 5; y++) 
    {
        int row = height * y / 5;
        
        const auto& localLuminances = source.getRow(row);
        
        int right = (width << 2) / 5;
        for (int x = width / 5; x < right; x++) 
        {
            int pixel = localLuminances[x] & 0xff;
            buckets[pixel >> LUMINANCE_SHIFT]++;
        }
    }

    int blackPoint = estimateBlackPoint();

    const auto& localLuminances = source.getMatrix();
    for (int y = 0; y < height; y++) 
    {
        int offset = y * width;
        for (int x = 0; x < width; x++) 
        {
            int pixel = localLuminances[offset + x] & 0xff;
            if (pixel < blackPoint) 
            {
                matrix.set(x, y);
            }
        }
    }
  
    return matrix;
}

int GlobalHistogramBinarizer::estimateBlackPoint()
{
    // Find tallest peak in histogram
    int numBuckets = buckets.size();
    int maxBucketCount = 0;
    int firstPeak = 0;
    int firstPeakSize = 0;

    for (int x = 0; x < numBuckets; x++) 
    {
        if (buckets[x] > firstPeakSize) 
        {
            firstPeak = x;
            firstPeakSize = buckets[x];
        }
        if (buckets[x] > maxBucketCount) 
        {
            maxBucketCount = buckets[x];
        }
    }

    // Find second-tallest peak -- well, another peak that is tall and not
    // so close to the first one
    int secondPeak = 0;
    int secondPeakScore = 0;

    for (int x = 0; x < numBuckets; x++) 
    {
        int distanceToBiggest = x - firstPeak;
        // Encourage more distant second peaks by multiplying by square of distance
        int score = buckets[x] * distanceToBiggest * distanceToBiggest;
        if (score > secondPeakScore) 
        {
            secondPeak = x;
            secondPeakScore = score;
        }
    }

    if (firstPeak > secondPeak) 
    {
        int temp = firstPeak;
        firstPeak = secondPeak;
        secondPeak = temp;
    }

    // Kind of arbitrary; if the two peaks are very close, then we figure there is
    // so little dynamic range in the image, that discriminating black and white
    // is too error-prone.
    // Decoding the image/line is either pointless, or may in some cases lead to
    // a false positive for 1D formats, which are relatively lenient.
    // We arbitrarily say "close" is
    // "<= 1/16 of the total histogram buckets apart"
    if (secondPeak - firstPeak <= numBuckets >> 4) 
    {
        return std::numeric_limits<int>::max();
    }

    // Find a valley between them that is low and closer to the white peak
    int bestValley = secondPeak - 1;
    int bestValleyScore = -1;
    for (int x = secondPeak - 1; x > firstPeak; x--) 
    {
        int fromFirst = x - firstPeak;
        // Favor a "valley" that is not too close to either peak -- especially not
        // the black peak -- and that has a low value of course
        int score = fromFirst * fromFirst * (secondPeak - x) * (maxBucketCount - buckets[x]);
        if (score > bestValleyScore) 
        {
            bestValley = x;
            bestValleyScore = score;
        }
    }

    return bestValley << LUMINANCE_SHIFT;
}

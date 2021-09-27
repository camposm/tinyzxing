/*
 *  Detector.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 14/05/2008.
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

#include <zxing/qrcode/detector/Detector.h>
#include <zxing/qrcode/detector/FinderPatternFinder.h>
#include <zxing/qrcode/detector/FinderPattern.h>
#include <zxing/qrcode/detector/AlignmentPattern.h>
#include <zxing/qrcode/detector/AlignmentPatternFinder.h>
#include <zxing/qrcode/Version.h>
#include <zxing/common/GridSampler.h>
#include <zxing/common/detector/MathUtils.h>
#include <cstdlib>
#include <limits>
#include <zxing/ZXing.h>


using std::abs;
using std::min;
using std::max;
using zxing::qrcode::Detector;
using zxing::bitmatrix_i;
using zxing::sampled_grid_t;
using zxing::DetectorResult;
using zxing::PerspectiveTransform;
using zxing::qrcode::AlignmentPattern;
using zxing::common::detector::MathUtils;


Detector::Detector (const bitmatrix_i& image) :
    image_(image) 
{
    // blank
}

const bitmatrix_i& Detector::getImage() const 
{
    return image_;
}

std::optional<DetectorResult> Detector::detect() 
{
    FinderPatternFinder finder(image_);
    FinderPatternInfo info(finder.find());
    
    return processFinderPatternInfo(info);
}

std::optional<DetectorResult> Detector::processFinderPatternInfo (const FinderPatternInfo& info)
{
    const FinderPattern& topLeft = info.getTopLeft();
    const FinderPattern& topRight = info.getTopRight();
    const FinderPattern& bottomLeft = info.getBottomLeft();

    float moduleSize = calculateModuleSize(topLeft, topRight, bottomLeft);
    if (moduleSize < 1.0f) 
    {
        return {};
    }
        
    int dimension = computeDimension(topLeft, topRight, bottomLeft, moduleSize);
        
    if (dimension <= 0)
    {
        return {};
    }
    
    const Version* provisionalVersion = Version::getProvisionalVersionForDimension(dimension);
    if (provisionalVersion == nullptr)
    {
        return {};
    }    
    
    int modulesBetweenFPCenters = provisionalVersion->getDimensionForVersion() - 7;

    std::optional<AlignmentPattern> alignmentPattern;
    
    // Anything above version 1 has an alignment pattern
    if (provisionalVersion->getAlignmentPatternCenters().size() > 0) 
    {
        // Guess where a "bottom right" finder pattern would have been
        float bottomRightX = topRight.first - topLeft.first + bottomLeft.first;
        float bottomRightY = topRight.second - topLeft.second + bottomLeft.second;

        // Estimate that alignment pattern is closer by 3 modules
        // from "bottom right" to known top left location
        float correctionToTopLeft = 1.0f - 3.0f / (float)modulesBetweenFPCenters;
        int estAlignmentX = (int)(topLeft.first + correctionToTopLeft * (bottomRightX - topLeft.first));
        int estAlignmentY = (int)(topLeft.second + correctionToTopLeft * (bottomRightY - topLeft.second));

        // Kind of arbitrary -- expand search radius before giving up
        for (int i = 4; i <= 16; i <<= 1) 
        {
            alignmentPattern = findAlignmentInRegion(moduleSize, estAlignmentX, estAlignmentY, (float)i);
            if (alignmentPattern)
            {
                break;
            } 
        }
    }
    
    PerspectiveTransform transform = createTransform(topLeft, topRight, bottomLeft, alignmentPattern, dimension);
    sampled_grid_t bits = sampleGrid(image_, dimension, transform);
    
    if (alignmentPattern)
    {
        return DetectorResult(bits, std::array<ResultPoint, 4>{bottomLeft, topLeft, topRight, alignmentPattern.value()});
    }
    else
    {
        return DetectorResult(bits, std::array<ResultPoint, 4>{bottomLeft, topLeft, topRight, ResultPoint(-1, -1)});
    }
}

PerspectiveTransform Detector::createTransform(
    const ResultPoint& topLeft, 
    const ResultPoint& topRight, 
    const ResultPoint& bottomLeft, 
    const std::optional<ResultPoint>& alignmentPattern, 
    int dimension) 
{

    float dimMinusThree = (float)dimension - 3.5f;
    float bottomRightX;
    float bottomRightY;
    float sourceBottomRightX;
    float sourceBottomRightY;

    if (alignmentPattern) 
    {
        bottomRightX = alignmentPattern.value().first;
        bottomRightY = alignmentPattern.value().second;
        sourceBottomRightX = dimMinusThree - 3.0f;
        sourceBottomRightY = sourceBottomRightX;
    } 
    else 
    {
        // Don't have an alignment pattern, just make up the bottom-right point
        bottomRightX = (topRight.first - topLeft.first) + bottomLeft.first;
        bottomRightY = (topRight.second - topLeft.second) + bottomLeft.second;
        sourceBottomRightX = dimMinusThree;
        sourceBottomRightY = dimMinusThree;
    }

    return PerspectiveTransform::quadrilateralToQuadrilateral(
        3.5f, 3.5f, 
        dimMinusThree, 3.5f, 
        sourceBottomRightX, sourceBottomRightY, 
        3.5f, dimMinusThree, 
        topLeft.first, topLeft.second, 
        topRight.first, topRight.second, 
        bottomRightX, bottomRightY, 
        bottomLeft.first, bottomLeft.second
    );
}

sampled_grid_t Detector::sampleGrid(const bitmatrix_i& image, int dimension, const PerspectiveTransform& transform) 
{
    GridSampler &sampler = GridSampler::getInstance();
    return sampler.sampleGrid(image, dimension, transform);
}

int Detector::computeDimension(
    const ResultPoint& topLeft, 
    const ResultPoint& topRight, 
    const ResultPoint& bottomLeft,
    float moduleSize) 
{
    int tltrCentersDimension = MathUtils::round(resultpoint::distance(topLeft, topRight) / moduleSize);
    int tlblCentersDimension = MathUtils::round(resultpoint::distance(topLeft, bottomLeft) / moduleSize);
    int dimension = ((tltrCentersDimension + tlblCentersDimension) >> 1) + 7;
    
    switch (dimension & 0x03) 
    { 
        // mod 4
        case 0:
        {
            dimension++;
            break;
        }
        case 1:
        {
            // 1? do nothing
            break;
        }
        case 2:
        {
            dimension--;
            break;
        }
        case 3:
        {
            // bad dimension
            dimension = -1;
        }
    }
  
    return dimension;
}

float Detector::calculateModuleSize(
    const ResultPoint& topLeft, 
    const ResultPoint& topRight, 
    const ResultPoint& bottomLeft) 
{
    // Take the average
    return (calculateModuleSizeOneWay(topLeft, topRight) + calculateModuleSizeOneWay(topLeft, bottomLeft)) / 2.0f;
}

float Detector::calculateModuleSizeOneWay (const ResultPoint& pattern, const ResultPoint& otherPattern) 
{
    float moduleSizeEst1 = sizeOfBlackWhiteBlackRunBothWays(
        (int)pattern.first, (int)pattern.second,
        (int)otherPattern.first, (int)otherPattern.second
    );
    
    float moduleSizeEst2 = sizeOfBlackWhiteBlackRunBothWays(
        (int)otherPattern.first, (int)otherPattern.second,
        (int)pattern.first, (int)pattern.second
    );
    
    if (zxing::isnan(moduleSizeEst1)) 
    {
        return moduleSizeEst2;
    }
    
    if (zxing::isnan(moduleSizeEst2)) 
    {
        return moduleSizeEst1;
    }
  
    // Average them, and divide by 7 since we've counted the width of 3 black modules,
    // and 1 white and 1 black module on either side. Ergo, divide sum by 14.
    return (moduleSizeEst1 + moduleSizeEst2) / 14.0f;
}

float Detector::sizeOfBlackWhiteBlackRunBothWays(int fromX, int fromY, int toX, int toY) 
{
    float result = sizeOfBlackWhiteBlackRun(fromX, fromY, toX, toY);

    // Now count other way -- don't run off image though of course
    float scale = 1.0f;
    int otherToX = fromX - (toX - fromX);
    if (otherToX < 0) 
    {
        scale = (float) fromX / (float) (fromX - otherToX);
        otherToX = 0;
    } 
    else if (otherToX >= (int)image_.getWidth()) 
    {
        scale = (float) (image_.getWidth() - 1 - fromX) / (float) (otherToX - fromX);
        otherToX = image_.getWidth() - 1;
    }
    
    int otherToY = (int) (fromY - (toY - fromY) * scale);
    scale = 1.0f;
    
    if (otherToY < 0) 
    {
        scale = (float) fromY / (float) (fromY - otherToY);
        otherToY = 0;
    } 
    else if (otherToY >= (int)image_.getHeight()) 
    {
        scale = (float) (image_.getHeight() - 1 - fromY) / (float) (otherToY - fromY);
        otherToY = image_.getHeight() - 1;
    }
    
    otherToX = (int) (fromX + (otherToX - fromX) * scale);
    result += sizeOfBlackWhiteBlackRun(fromX, fromY, otherToX, otherToY);

    // Middle pixel is double-counted this way; subtract 1
    return result - 1.0f;
}

float Detector::sizeOfBlackWhiteBlackRun(int fromX, int fromY, int toX, int toY) 
{
    // Mild variant of Bresenham's algorithm;
    // see http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
    bool steep = abs(toY - fromY) > abs(toX - fromX);
    
    if (steep) 
    {
        int temp = fromX;
        fromX = fromY;
        fromY = temp;
        temp = toX;
        toX = toY;
        toY = temp;
    }

    int dx = abs(toX - fromX);
    int dy = abs(toY - fromY);
    int error = -dx >> 1;
    int xstep = fromX < toX ? 1 : -1;
    int ystep = fromY < toY ? 1 : -1;

    // In black pixels, looking for white, first or second time.
    int state = 0;
    // Loop up until x == toX, but not beyond
    int xLimit = toX + xstep;
    for (int x = fromX, y = fromY; x != xLimit; x += xstep) 
    {
        int realX = steep ? y : x;
        int realY = steep ? x : y;

        // Does current pixel mean we have moved white to black or vice versa?
        if (!((state == 1) ^ image_.get(realX, realY))) 
        {
            if (state == 2) 
            {
                return MathUtils::distance(x, y, fromX, fromY);
            }
            state++;
        }

        error += dy;
        if (error > 0) 
        {
            if (y == toY) 
            {
                break;
            }
            y += ystep;
            error -= dx;
        }
    }
    
    // Found black-white-black; give the benefit of the doubt that the next pixel outside the image
    // is "white" so this last point at (toX+xStep,toY) is the right ending. This is really a
    // small approximation; (toX+xStep,toY+yStep) might be really correct. Ignore this.
    if (state == 2) 
    {
        return MathUtils::distance(toX + xstep, toY, fromX, fromY);
    }
    
    // else we didn't find even black-white-black; no estimate is really possible
    return nan();
}

std::optional<AlignmentPattern> Detector::findAlignmentInRegion(
    float overallEstModuleSize, 
    int estAlignmentX, 
    int estAlignmentY,
    float allowanceFactor) 
{
    // Look for an alignment pattern (3 modules in size) around where it
    // should be
    int allowance = (int)(allowanceFactor * overallEstModuleSize);
    int alignmentAreaLeftX = max(0, estAlignmentX - allowance);
    int alignmentAreaRightX = min((int)(image_.getWidth() - 1), estAlignmentX + allowance);
    
    if (alignmentAreaRightX - alignmentAreaLeftX < overallEstModuleSize * 3) 
    {
        return {};
    }
    
    int alignmentAreaTopY = max(0, estAlignmentY - allowance);
    int alignmentAreaBottomY = min((int)(image_.getHeight() - 1), estAlignmentY + allowance);
    
    if (alignmentAreaBottomY - alignmentAreaTopY < overallEstModuleSize * 3) 
    {
        return {};
    }

    AlignmentPatternFinder alignmentFinder(
        image_, 
        alignmentAreaLeftX, 
        alignmentAreaTopY, 
        alignmentAreaRightX - alignmentAreaLeftX, 
        alignmentAreaBottomY - alignmentAreaTopY, 
        overallEstModuleSize
    );
  
    return alignmentFinder.find();
}

#ifndef __DETECTOR_H__
#define __DETECTOR_H__

/*
 *  Detector.h
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

#include <optional>
#include <zxing/common/GridSampler.h>
#include <zxing/common/DetectorResult.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/qrcode/detector/AlignmentPattern.h>
#include <zxing/common/PerspectiveTransform.h>
#include <zxing/qrcode/detector/FinderPatternInfo.h>

namespace zxing {
namespace qrcode {

class Detector 
{
public:
    explicit Detector (const bitmatrix_i& image);

    std::optional<DetectorResult> detect();
    
    virtual PerspectiveTransform createTransform(
        const ResultPoint& topLeft, 
        const ResultPoint& topRight, 
        const ResultPoint& bottomLeft, 
        const std::optional<ResultPoint>& alignmentPattern, 
        int dimension
    ); 

protected:
    const bitmatrix_i& getImage() const;
    static sampled_grid_t sampleGrid(const bitmatrix_i& image, int dimension, const PerspectiveTransform& pt);
    
    static int computeDimension(
        const ResultPoint& topLeft, 
        const ResultPoint& topRight, 
        const ResultPoint& bottomLeft,
        float moduleSize
    );
    
    float calculateModuleSize(
        const ResultPoint& topLeft, 
        const ResultPoint& topRight, 
        const ResultPoint& bottomLeft
    );
     
    float calculateModuleSizeOneWay (const ResultPoint& pattern, const ResultPoint& otherPattern);
    float sizeOfBlackWhiteBlackRunBothWays(int fromX, int fromY, int toX, int toY);
    float sizeOfBlackWhiteBlackRun(int fromX, int fromY, int toX, int toY);
    
    std::optional<AlignmentPattern> findAlignmentInRegion(
        float overallEstModuleSize, 
        int estAlignmentX, 
        int estAlignmentY,
        float allowanceFactor
    );
    
    std::optional<DetectorResult> processFinderPatternInfo (const FinderPatternInfo& info);

private:
    const bitmatrix_i& image_;

};

} // ns qrcode
} // ns zxing

#endif // __DETECTOR_H__

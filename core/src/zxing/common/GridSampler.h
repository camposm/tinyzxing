#ifndef __GRID_SAMPLER_H__
#define __GRID_SAMPLER_H__

/*
 *  GridSampler.h
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

#include <zxing/ZXingConfig.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/common/PerspectiveTransform.h>

namespace zxing {

using sampled_grid_t = bitmatrix_t<config::MaxGridSize, config::MaxGridSize>;

class GridSampler 
{
public:
    static GridSampler &getInstance();

    sampled_grid_t sampleGrid(const bitmatrix_i& image, int dimension, const PerspectiveTransform& transform);
    
    static void checkAndNudgePoints(const bitmatrix_i&, gs_points_t& points, const int used_points);

private:
    static GridSampler gridSampler;
    GridSampler() = default;
};

} // ns zxing

#endif // __GRID_SAMPLER_H__

/*
 *  GridSampler.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 18/05/2008.
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

#include <zxing/common/GridSampler.h>
#include <zxing/common/PerspectiveTransform.h>

namespace zxing {

GridSampler GridSampler::gridSampler;


sampled_grid_t GridSampler::sampleGrid (const bitmatrix_i& image, int dimension, const PerspectiveTransform& transform) 
{
    sampled_grid_t bits(dimension);
    gs_points_t points = {0};

    const auto points_size = dimension << 1;
  
    for (int y = 0; y < dimension; y++) 
    {
        float yValue = y + 0.5f;

        for (int x = 0; x < points_size; x += 2) 
        {
            points[x] = (x >> 1) + 0.5f;
            points[x + 1] = yValue;
        }
    
        transform.transformPoints(points);
        checkAndNudgePoints(image, points, points_size);
        
        for (int x = 0; x < points_size; x += 2) 
        {
            if (image.get(static_cast<int>(points[x]), static_cast<int>(points[x + 1]))) 
            {
                bits.set(x >> 1, y);
            }
        }
    }
    
    return std::move(bits);
}

void GridSampler::checkAndNudgePoints (const bitmatrix_i& image, gs_points_t& points, const int used_points) 
{
    const auto width = image.getWidth();
    const auto height = image.getHeight();

    // The Java code assumes that if the start and end points are in bounds, the rest will also be.
    // However, in some unusual cases points in the middle may also be out of bounds.
    // Since we can't rely on an ArrayIndexOutOfBoundsException like Java, we check every point.

    for (size_t offset = 0; offset < used_points; offset += 2) 
    {
        int x = (int)points[offset];
        int y = (int)points[offset + 1];

        if (x < 0) 
        {
            points[offset] = 0.0f;
        } 
        else if (x >= width) 
        {
            points[offset] = float(width - 1);
        }
        
        if (y < 0) 
        {
            points[offset + 1] = 0.0f;
        }
        else if (y >= height) 
        {
            points[offset + 1] = float(height - 1);
        }
    }
}

GridSampler &GridSampler::getInstance() 
{
    return gridSampler;
}

}

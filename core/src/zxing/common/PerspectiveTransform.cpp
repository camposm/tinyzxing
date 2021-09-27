/*
 *  PerspectiveTransform.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 12/05/2008.
 *  Copyright 2008 Google UK. All rights reserved.
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

#include <zxing/common/PerspectiveTransform.h>

namespace zxing {


PerspectiveTransform::PerspectiveTransform(
    float inA11, float inA21, float inA31, 
    float inA12, float inA22, float inA32, 
    float inA13, float inA23, float inA33
) : 
    a11(inA11), a12(inA12), a13(inA13), 
    a21(inA21), a22(inA22), a23(inA23),
    a31(inA31), a32(inA32), a33(inA33) 
{
    // blank
}

PerspectiveTransform PerspectiveTransform::quadrilateralToQuadrilateral(
    float x0, float y0, 
    float x1, float y1,
    float x2, float y2, 
    float x3, float y3, 
    float x0p, float y0p, 
    float x1p, float y1p, 
    float x2p, float y2p,
    float x3p, float y3p) 
{
    PerspectiveTransform qToS = PerspectiveTransform::quadrilateralToSquare(x0, y0, x1, y1, x2, y2, x3, y3);
    PerspectiveTransform sToQ = PerspectiveTransform::squareToQuadrilateral(x0p, y0p, x1p, y1p, x2p, y2p, x3p, y3p);
  
    return sToQ.times(qToS);
}

PerspectiveTransform PerspectiveTransform::squareToQuadrilateral(
    float x0, float y0, 
    float x1, float y1, 
    float x2, float y2, 
    float x3, float y3) 
{
    float dx3 = x0 - x1 + x2 - x3;
    float dy3 = y0 - y1 + y2 - y3;

    float result[9] = {0};

    if (dx3 == 0.0f && dy3 == 0.0f) // hmmm...
    {
        result[0] = x1 - x0;
        result[1] = x2 - x1;
        result[2] = x0;
        result[3] = y1 - y0; 
        result[4] = y2 - y1;
        result[5] = y0;
        result[6] = 0.0f; 
        result[7] = 0.0f; 
        result[8] = 1.0f;
    }
    else 
    {
        float dx1 = x1 - x2;
        float dx2 = x3 - x2;
        float dy1 = y1 - y2;
        float dy2 = y3 - y2;
        float denominator = dx1 * dy2 - dx2 * dy1;
        float a13 = (dx3 * dy2 - dx2 * dy3) / denominator;
        float a23 = (dx1 * dy3 - dx3 * dy1) / denominator;
        
        result[0] = x1 - x0 + a13 * x1;
        result[1] = x3 - x0 + a23 * x3;
        result[2] = x0;
        result[3] = y1 - y0 + a13 * y1;
        result[4] = y3 - y0 + a23 * y3;
        result[5] = y0;
        result[6] = a13;
        result[7] = a23;
        result[8] = 1.0f;
    }
    
    return PerspectiveTransform(
        result[0], result[1], result[2],
        result[3], result[4], result[5],
        result[6], result[7], result[8]
    );
}

PerspectiveTransform PerspectiveTransform::quadrilateralToSquare(
    float x0, float y0, 
    float x1, float y1, 
    float x2, float y2, 
    float x3, float y3) 
{
    // Here, the adjoint serves as the inverse:
    return squareToQuadrilateral(x0, y0, x1, y1, x2, y2, x3, y3).buildAdjoint();
}

PerspectiveTransform PerspectiveTransform::buildAdjoint() 
{
    // Adjoint is the transpose of the cofactor matrix:
    return PerspectiveTransform(
        a22 * a33 - a23 * a32,
        a23 * a31 - a21 * a33, 
        a21 * a32 - a22 * a31, 
        a13 * a32 - a12 * a33, 
        a11 * a33 - a13 * a31, 
        a12 * a31 - a11 * a32, 
        a12 * a23 - a13 * a22,
        a13 * a21 - a11 * a23, 
        a11 * a22 - a12 * a21
    );
}

PerspectiveTransform PerspectiveTransform::times (const PerspectiveTransform& other) 
{
    return PerspectiveTransform(
        a11 * other.a11 + a21 * other.a12 + a31 * other.a13,
        a11 * other.a21 + a21 * other.a22 + a31 * other.a23, 
        a11 * other.a31 + a21 * other.a32 + a31 * other.a33, 
        a12 * other.a11 + a22 * other.a12 + a32 * other.a13, 
        a12 * other.a21 + a22 * other.a22 + a32 * other.a23, 
        a12 * other.a31 + a22 * other.a32 + a32 * other.a33, 
        a13 * other.a11 + a23 * other.a12 + a33 * other.a13, 
        a13 * other.a21 + a23 * other.a22 + a33 * other.a23, 
        a13 * other.a31 + a23 * other.a32 + a33 * other.a33
    );
}

void PerspectiveTransform::transformPoints (gs_points_t& points) const
{
    const int points_size = points.size();
    for (int i = 0; i < points_size; i += 2) 
    {
        float x = points[i];
        float y = points[i + 1];
        float denominator = a13 * x + a23 * y + a33;
        
        points[i] = (a11 * x + a21 * y + a31) / denominator;
        points[i + 1] = (a12 * x + a22 * y + a32) / denominator;
    }
}

} // zxing

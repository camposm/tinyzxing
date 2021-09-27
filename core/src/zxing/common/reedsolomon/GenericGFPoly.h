/*
 *  GenericGFPoly.h
 *  zxing
 *
 *  Created by Lukas Stabe on 13/02/2012.
 *  Copyright 2012 ZXing authors All rights reserved.
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

#ifndef GENERICGFPOLY_H
#define GENERICGFPOLY_H

#include <array>
#include <zxing/common/reedsolomon/GenericGF.h>


namespace zxing {

constexpr uint32_t max_poly_size = 64; // TODO: calculate from some parameter
constexpr uint32_t max_res_size = 8; // TODO: calculate from some parameter

struct my_GenericGFPoly
{
    explicit my_GenericGFPoly (const GenericGF_i& field) : coeff{0}, size(1), field(field) {}

    static my_GenericGFPoly buildMonomial (const GenericGF_i& field, int degree, int coefficient) 
    {
        if (coefficient == 0) 
        {
            return my_GenericGFPoly(field);
        }
  
        auto p = my_GenericGFPoly(field);
        p.size = degree + 1;
        p.coeff[0] = coefficient;
        
        return p;
    }

    my_GenericGFPoly& operator= (const zxing::my_GenericGFPoly& other)
    {
        size = other.size;
        for (auto n = 0; n < other.size; ++n)
        {
            coeff[n] = other.coeff[n];
        }
        return *this;
    }

    const std::array<int, max_poly_size>& getCoefficients() const
    {
        return coeff;
    }
  
    int getDegree() const
    {
        return size - 1;
    }
    
    bool isZero() const
    {
        return coeff[0] == 0;
    }
    
    int getCoefficient (int degree) const
    {
        return coeff[size - 1 - degree];
    }
    
    int evaluateAt (int a) const;
    my_GenericGFPoly addOrSubtract (const my_GenericGFPoly& other) const;
    my_GenericGFPoly multiply (const my_GenericGFPoly& other) const;
    my_GenericGFPoly multiply (int scalar) const;
    my_GenericGFPoly multiplyByMonomial (int degree, int coefficient) const;

    void removeLeadingZeros();

    std::array<int, max_poly_size>  coeff;
    int                             size;
    const GenericGF_i&              field;
};

} // ns zxing

#endif //GENERICGFPOLY_H

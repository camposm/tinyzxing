/*
 *  GenericGFPoly.cpp
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

#include <zxing/common/reedsolomon/GenericGFPoly.h>

using zxing::my_GenericGFPoly;
using zxing::GenericGF_i;


int my_GenericGFPoly::evaluateAt (int a) const
{
    if (a == 0) 
    {
        // Just return the x^0 coefficient
        return getCoefficient(0);
    }
    
    if (a == 1) 
    {
        // Just the sum of the coefficients
        int result = 0;
        for (int i = 0; i < size; i++) 
        {
            result = result ^ coeff[i];
        }
        return result;
    }
    
    int result = coeff[0];
    for (int i = 1; i < size; i++) 
    {
        result = field.multiply(a, result) ^ coeff[i];
    }
    
    return result;
}

my_GenericGFPoly my_GenericGFPoly::addOrSubtract (const my_GenericGFPoly& other) const
{
    if (isZero()) 
    {
        return other;
    }
    
    if (other.isZero())
    {
        return *this;
    }
    
    const auto* sc = this;
    const auto* lc = &other;
    
    if (size > other.size)
    {
        const auto* tmp = sc;
        sc = lc;
        lc = tmp;
    }    
    
    int lengthDiff = lc->size - sc->size;
    
    auto p = my_GenericGFPoly(field);
    p.size = lc->size;

    // Copy high-order terms only found in higher-degree polynomial's coefficients    
    for (int i = 0; i < lengthDiff; ++i) 
    {
        p.coeff[i] = lc->coeff[i];
    }
    
    for (int i = lengthDiff; i < lc->size; ++i) 
    {
        p.coeff[i] = sc->coeff[i-lengthDiff] ^ lc->coeff[i];
    }
    
    p.removeLeadingZeros();
    return p;
}

my_GenericGFPoly my_GenericGFPoly::multiply (const my_GenericGFPoly& other) const
{
    if (isZero() || other.isZero()) 
    {
        return my_GenericGFPoly(field);
    }
    
    auto p = my_GenericGFPoly(field);
    
    if ((size + other.size - 1) >= max_poly_size)
    {
        p.size = -1;
        return p;
    }
    
    p.size = size + other.size - 1;
    
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < other.size; ++j)
        {
            p.coeff[i+j] = p.coeff[i+j] ^ field.multiply(coeff[i], other.coeff[j]);
        }
    }
    
    p.removeLeadingZeros();
    return p;
}

my_GenericGFPoly my_GenericGFPoly::multiply (int scalar) const
{
    if (scalar == 0) 
    {
        return my_GenericGFPoly(field);
    }
    
    if (scalar == 1) 
    {
        return *this;
    }

    auto p = my_GenericGFPoly(field);
    p.size = size;
    
    for (int i = 0; i < size; ++i) 
    {
        p.coeff[i] = field.multiply(coeff[i], scalar);
    }
  
    p.removeLeadingZeros();
    return p;
}

my_GenericGFPoly my_GenericGFPoly::multiplyByMonomial (int degree, int coefficient) const
{
    auto p = my_GenericGFPoly(field);
    
    if (degree < 0) 
    {
        p.size = -1;
        return p;
    }
    
    if (coefficient == 0) 
    {
        return p;
    }
    
    p.size = size + degree;
    for (int i = 0; i < size; ++i) 
    {
        p.coeff[i] = field.multiply(coeff[i], coefficient);
    }
  
    p.removeLeadingZeros();
    return p;
}

void my_GenericGFPoly::removeLeadingZeros()
{
    if (size > 1 && coeff[0] == 0) 
    {
        // Leading term must be non-zero for anything except the constant polynomial "0"
        int firstNonZero = 1;
        while (firstNonZero < size && coeff[firstNonZero] == 0) 
        {
            firstNonZero++;
        }
        
        if (firstNonZero == size) 
        {
            size = 1;
        } 
        else 
        {
            int delta = size - firstNonZero;
            for (int i = 0; i < delta; ++i)
            {
                coeff[i] = coeff[i + firstNonZero];
            }
            size = delta;
        }
    } 
}

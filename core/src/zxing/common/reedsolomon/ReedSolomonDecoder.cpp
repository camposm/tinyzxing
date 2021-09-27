/*
 *  Created by Christian Brunschen on 05/05/2008.
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


#include <zxing/common/reedsolomon/ReedSolomonDecoder.h>

using zxing::ReedSolomonDecoder;
using zxing::my_GenericGFPoly;


ReedSolomonDecoder::ReedSolomonDecoder (const GenericGF_i& field) :
    field(field)
{
    // blank
}

bool ReedSolomonDecoder::decode (std::array<char, zxing::config::some_number_of_codewords>& received, const int numCodewords, const int twoS)
{
    auto poly = my_GenericGFPoly(field);
    
    if (numCodewords > zxing::max_poly_size)
    {
        //std::cout << "E: lib is misconfigured: max_poly_size=" << zxing::max_poly_size << " numCodewords=" << numCodewords << "\n";
        return true;
    }
    
    poly.size = numCodewords;
    for (int n = 0; n < numCodewords; ++n)
    {
        poly.coeff[n] = (unsigned char)received[n];
    }

    auto syndrome = my_GenericGFPoly(field);
    if (twoS > zxing::max_poly_size)
    {
        //std::cout << "E: lib is misconfigured: max_poly_size=" << zxing::max_poly_size << " twoS=" << twoS << "\n";
        return true;
    }
    syndrome.size = twoS;
    
    bool noError = true;
    for (int i = 0; i < twoS; ++i) 
    {
        int eval = poly.evaluateAt(field.exp(i + field.getGeneratorBase()));
        syndrome.coeff[twoS - 1 - i] = eval;
        if (eval != 0) 
        {
            noError = false;
        }
    }
  
    if (noError)
    {
        return false;
    }
    
    auto mon = my_GenericGFPoly::buildMonomial(field, twoS, 1);
    auto sigmaOmega = runEuclideanAlgorithm(mon, syndrome, twoS);
    
    if (sigmaOmega.first.getDegree() > max_res_size)
    {
        // too many errors (value chosen arbitrarily; check the parameters for Reed-Solomon)
        return true;
    }

    auto errorLocations = findErrorLocations(sigmaOmega.first);
    if (errorLocations.size < 0)
    {
        return true;
    }
    
    auto errorMagitudes = findErrorMagnitudes(sigmaOmega.second, errorLocations);

    for (int i = 0; i < errorLocations.size; i++) 
    {
        int position = numCodewords - 1 - field.log(errorLocations.values[i]);
        if (position < 0) 
        {
            return true;
        }
        received[position] = received[position] ^ errorMagitudes.values[i];
    }
    
    return false;
}

std::pair<zxing::my_GenericGFPoly, zxing::my_GenericGFPoly> ReedSolomonDecoder::runEuclideanAlgorithm(
    const zxing::my_GenericGFPoly& a, 
    const zxing::my_GenericGFPoly& b, 
    int R
)
{
    auto pa(a);
    auto pb(b);
    
    if (a.getDegree() < b.getDegree())
    {
        auto tmp = pa;
        pa = pb;
        pb = tmp;
    }

    auto rLast(pa);
    auto r(pb);
    
    // Zero
    auto zero = my_GenericGFPoly(a.field);
    zero.size = 1;
    // One
    auto one = my_GenericGFPoly(a.field);
    one.size = 1;
    one.coeff[0] = 1;
    
    auto tLast = zero;
    auto t = one;
    
    auto sigma = my_GenericGFPoly(a.field);
    auto omega = my_GenericGFPoly(a.field);    
    
    // Run Euclidean algorithm until r's degree is less than R/2
    while (r.getDegree() >= R / 2) 
    {
        auto rLastLast = rLast;
        auto tLastLast = tLast;
        rLast = r;
        tLast = t;

        // Divide rLastLast by rLast, with quotient q and remainder r
        if (rLast.isZero())
        {
            // Oops, Euclidean algorithm already terminated?
            sigma.size = -1;
            omega.size = -1;
            return {sigma, omega};
        }
    
        r = rLastLast;
        auto q = zero;
        
        int denominatorLeadingTerm = rLast.getCoefficient(rLast.getDegree());
        int dltInverse = field.inverse(denominatorLeadingTerm);
    
        while (r.getDegree() >= rLast.getDegree() && !r.isZero()) 
        {
            int degreeDiff = r.getDegree() - rLast.getDegree();
            int scale = field.multiply(r.getCoefficient(r.getDegree()), dltInverse);
            q = q.addOrSubtract(my_GenericGFPoly::buildMonomial(field, degreeDiff, scale));
            r = r.addOrSubtract(rLast.multiplyByMonomial(degreeDiff, scale));
        }

        t = q.multiply(tLast).addOrSubtract(tLastLast);

        if (r.getDegree() >= rLast.getDegree()) 
        {
            sigma.size = -1;
            omega.size = -1;
            return {sigma, omega};
        }
    }

    int sigmaTildeAtZero = t.getCoefficient(0);
    if (sigmaTildeAtZero == 0) 
    {
        sigma.size = -1;
        omega.size = -1;
        return {sigma, omega};
    }

    int inverse = field.inverse(sigmaTildeAtZero);
    sigma = t.multiply(inverse);
    omega = r.multiply(inverse);
    
    return {sigma, omega};
}

ReedSolomonDecoder::error_result ReedSolomonDecoder::findErrorLocations (const zxing::my_GenericGFPoly& errorLocator)
{
    // This is a direct application of Chien's search
    int numErrors = errorLocator.getDegree();
    error_result result = {0};
    
    if (numErrors == 1)
    {
        result.values[0] = errorLocator.getCoefficient(1);
        result.size = 1;
        return result;
    }

    result.size = 0;
    for (int i = 1; i < field.getSize() && result.size < numErrors; ++i) 
    {
        if (errorLocator.evaluateAt(i) == 0) 
        {
            result.values[result.size] = field.inverse(i);
            ++result.size;
        }
    }
    
    if (result.size != numErrors) 
    {
        result.size = -1;
    }

    return result;
}

ReedSolomonDecoder::error_result ReedSolomonDecoder::findErrorMagnitudes(
    const zxing::my_GenericGFPoly& errorEvaluator, 
    const ReedSolomonDecoder::error_result& errorLocations)
{
    error_result result = {0};
    
    int s = errorLocations.size;
    result.size = errorLocations.size;
    for (int i = 0; i < s; ++i) 
    {
        int xiInverse = field.inverse(errorLocations.values[i]);
        int denominator = 1;
        for (int j = 0; j < s; ++j) 
        {
            if (i != j) 
            {
                int term = field.multiply(errorLocations.values[j], xiInverse);
                int termPlus1 = (term & 0x1) == 0 ? term | 1 : term & ~1;
                denominator = field.multiply(denominator, termPlus1);
            }
        }
        
        result.values[i] = field.multiply(errorEvaluator.evaluateAt(xiInverse), field.inverse(denominator));
        if (field.getGeneratorBase() != 0) 
        {
            result.values[i] = field.multiply(result.values[i], xiInverse);
        }
    }
    
    return result;
}

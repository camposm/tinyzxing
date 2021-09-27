/*
 *  DecodedBitStreamParser.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 20/05/2008.
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

#include <zxing/qrcode/decoder/DecodedBitStreamParser.h>


using namespace zxing;
using namespace zxing::qrcode;


namespace {

void decodeByteSegment(
    bitsource_t<config::MaxDecodedOutput>& bits,
    zxing::config::decoded_output_t& result,
    int count) 
{
    // Don't crash trying to read more bits than we have available.
    if ((count << 3) > bits.availableBits()) 
    {
        // detection failed
        result.second = 0;
        return;
    }
  
    for (auto n = 0; n < count && result.second < zxing::config::MaxDecodedOutput; ++n)
    {
        result.first[result.second] = (char) bits.readBits(8);
        result.second += 1;
    }
}

} // ns -anon-

zxing::config::decoded_output_t DecodedBitStreamParser::decode (
    std::array<char, config::MaxDecodedOutput>& bytes, 
    const int input_size, 
    const Version* version, 
    const int ecLevel)
{
    bitsource_t<config::MaxDecodedOutput> bits(bytes, input_size);
    
    zxing::config::decoded_output_t decoded_data;

    const Mode* mode = nullptr;
    
    do 
    {
        // While still another segment to read...
        if (bits.availableBits() < 4) 
        {
            // OK, assume we're done. Really, a TERMINATOR mode should have been recorded here
            mode = &Mode::TERMINATOR;
        } 
        else 
        {
            mode = &Mode::forBits(bits.readBits(4)); // mode is encoded by 4 bits
            if (mode == &Mode::INVALID_MODE)
            {
                // decoding failed
                return {};
            }
        }
        
        if (mode != &Mode::TERMINATOR) 
        {
            if ((mode == &Mode::FNC1_FIRST_POSITION) || (mode == &Mode::FNC1_SECOND_POSITION)) 
            {
                // We do little with FNC1 except alter the parsed result a bit according to the spec
                // nothing to do
            } 
            else if (mode == &Mode::STRUCTURED_APPEND) 
            {
                if (bits.availableBits() < 16) 
                {
                    // decoding failed
                    return {};
                }
                
                // not really supported; all we do is ignore it
                // Read next 8 bits (symbol sequence #) and 8 bits (parity data), then continue
                bits.readBits(16);
            } 
            else if (mode == &Mode::ECI) 
            {
                // not supported, decoding failed
                return {};
            } 
            else 
            {
                // First handle Hanzi mode which does not start with character count
                if (mode == &Mode::HANZI) 
                {
                    // not supported, decoding failed
                    return {};
                } 
                else 
                {
                    // "Normal" QR code modes:
                    // How many characters will follow, encoded in this mode?
                    int count = bits.readBits(mode->getCharacterCountBits(version));
                    if (mode == &Mode::NUMERIC) 
                    {
                        // not supported, decoding failed
                        return {};
                    } 
                    else if (mode == &Mode::ALPHANUMERIC) 
                    {
                        // not supported, decoding failed
                        return {};
                    } 
                    else if (mode == &Mode::BYTE) 
                    {
                        decodeByteSegment(bits, decoded_data, count);
                    } 
                    else if (mode == &Mode::KANJI) 
                    {
                        // not supported, decoding failed
                        return {};
                    } 
                    else 
                    {
                        // wtf? decoding failed
                        return {};
                    }
                }
            }
        }
    } while (mode != &Mode::TERMINATOR);
   
    return decoded_data;
}


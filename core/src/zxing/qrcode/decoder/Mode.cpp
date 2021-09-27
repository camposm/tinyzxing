/*
 *  Mode.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 19/05/2008.
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

#include <zxing/ZXing.h>
#include <zxing/qrcode/decoder/Mode.h>


using zxing::qrcode::Mode;


const Mode Mode::TERMINATOR(0, 0, 0);
const Mode Mode::NUMERIC (10, 12, 14);
const Mode Mode::ALPHANUMERIC(9, 11, 13);
const Mode Mode::STRUCTURED_APPEND(0, 0, 0);
const Mode Mode::BYTE(8, 16, 16);
const Mode Mode::ECI(0, 0, 0);
const Mode Mode::KANJI(8, 10, 12);
const Mode Mode::FNC1_FIRST_POSITION(0, 0, 0);
const Mode Mode::FNC1_SECOND_POSITION(0, 0, 0);
const Mode Mode::HANZI(8, 10, 12);
const Mode Mode::INVALID_MODE(-1, -1, -1);

Mode::Mode(int cbv0_9, int cbv10_26, int cbv27) :
    characterCountBitsForVersions0To9_(cbv0_9), 
    characterCountBitsForVersions10To26_(cbv10_26),
    characterCountBitsForVersions27AndHigher_(cbv27)
{
    // blank
}

const Mode& Mode::forBits (int bits) 
{
    switch (bits) 
    {
        case 0x0: { return TERMINATOR; }
        case 0x1: { return NUMERIC; }
        case 0x2: { return ALPHANUMERIC; }
        case 0x3: { return STRUCTURED_APPEND; }
        case 0x4: { return BYTE; }
        case 0x5: { return FNC1_FIRST_POSITION; }
        case 0x7: { return ECI; }
        case 0x8: { return KANJI; }
        case 0x9: { return FNC1_SECOND_POSITION; }
        // 0xD is defined in GBT 18284-2000, may not be supported in foreign country
        case 0xD: { return HANZI; }
        default:  { return INVALID_MODE; }
    }
}

int Mode::getCharacterCountBits (const Version *version) const
{
    int number = version->getVersionNumber();
    
    if (number <= 9) 
    {
        return characterCountBitsForVersions0To9_;
    } 
    else if (number <= 26) 
    {
        return characterCountBitsForVersions10To26_;
    } 
    else 
    {
        return characterCountBitsForVersions27AndHigher_;
    }
}

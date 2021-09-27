#ifndef __MODE_H__
#define __MODE_H__

/*
 *  Mode.h
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
#include <zxing/qrcode/Version.h>

namespace zxing {
namespace qrcode {

class Mode 
{
public:
    static const Mode TERMINATOR;
    static const Mode NUMERIC;
    static const Mode ALPHANUMERIC;
    static const Mode STRUCTURED_APPEND;
    static const Mode BYTE;
    static const Mode ECI;
    static const Mode KANJI;
    static const Mode FNC1_FIRST_POSITION;
    static const Mode FNC1_SECOND_POSITION;
    static const Mode HANZI;
    static const Mode INVALID_MODE;

    static const Mode& forBits (int bits);
    int getCharacterCountBits (const Version *version) const;

private:
    int characterCountBitsForVersions0To9_;
    int characterCountBitsForVersions10To26_;
    int characterCountBitsForVersions27AndHigher_;

    Mode(int cbv0_9, int cbv10_26, int cbv27);

};

} // ns qrcode
} // ns zxing

#endif // __MODE_H__

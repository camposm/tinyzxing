#ifndef __ERROR_CORRECTION_LEVEL_H__
#define __ERROR_CORRECTION_LEVEL_H__

/*
 *  ErrorCorrectionLevel.h
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


#include <array>

namespace zxing {
namespace qrcode {

constexpr std::array<int, 4> ErrorCorrectionLevel = {
    0x01,   // L
    0x00,   // M
    0x03,   // Q
    0x02    // H
};

} // ns qrcode
} // ns zxing

#endif // __ERROR_CORRECTION_LEVEL_H__

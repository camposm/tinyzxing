#ifndef __FORMAT_INFORMATION_H__
#define __FORMAT_INFORMATION_H__

/*
 *  FormatInformation.h
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
#include <zxing/qrcode/ErrorCorrectionLevel.h>


namespace zxing {
namespace qrcode {

class FormatInformation 
{
public:
    static int numBitsDiffering (int a, int b);
    static std::optional<FormatInformation> decodeFormatInformation(int maskedFormatInfo1, int maskedFormatInfo2);
    static std::optional<FormatInformation> doDecodeFormatInformation(int maskedFormatInfo1, int maskedFormatInfo2);

    int getErrorCorrectionLevel() const;
    char getDataMask() const;

    friend bool operator==(const FormatInformation& a, const FormatInformation& b);

private:
    static const int FORMAT_INFO_MASK_QR;
    static const int FORMAT_INFO_DECODE_LOOKUP[][2];
    static const int N_FORMAT_INFO_DECODE_LOOKUPS;
    static const int BITS_SET_IN_HALF_BYTE[];

    int errorCorrectionLevel_;
    char dataMask_;

    FormatInformation(int formatInfo);
};

} // ns qrcode
} // ns zxing

#endif // __FORMAT_INFORMATION_H__

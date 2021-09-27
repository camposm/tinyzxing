/*
 *  QRCodeReader.cpp
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

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/qrcode/detector/Detector.h>


namespace zxing {
namespace qrcode {

QRCodeReader::QRCodeReader() : 
    decoder_() 
{
    // blank
}

config::decoded_output_t QRCodeReader::decode(BinaryBitmap& image) 
{
    Detector detector(image.getBlackMatrix());
    auto detectorResult = detector.detect();

    if (detectorResult)
    {
        //auto& points = detectorResult.value().getPoints();
        auto decoderResult = decoder_.decode(detectorResult.value().getBits());

        if (decoderResult.second != 0)
        {
            return decoderResult;
        }
        else
        {
            //std::cout << "decoding failed\n";
        }
    }
    else
    {
        //std::cout << "detection failed\n";
    }
    
    return {};
}

Decoder& QRCodeReader::getDecoder() 
{
    return decoder_;
}
    
} // ns qrcode
} // ns zxing

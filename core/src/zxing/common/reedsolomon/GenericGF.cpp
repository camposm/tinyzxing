/*
 *  GenericGF.cpp
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

#include <zxing/common/reedsolomon/GenericGF.h>
#include <zxing/common/reedsolomon/GenericGFPoly.h>

const zxing::my_GenericGF<0x011D, 256, 0> my_QR_CODE_FIELD_256;

namespace zxing {

const GenericGF_i& get_QR_Field()
{
    return my_QR_CODE_FIELD_256;
}

}

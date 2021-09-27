#ifndef __DATA_MASK_H__
#define __DATA_MASK_H__

/*
 *  DataMask.h
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
#include <functional>
#include <zxing/common/BitMatrix.h>
#include <zxing/common/GridSampler.h>


namespace zxing {
namespace qrcode {

using mask_fn = std::function<bool(size_t,size_t)>;

class DataMask
{
public:
    static std::optional<DataMask> select (int reference);

    void unmaskBitMatrix (sampled_grid_t& matrix, size_t dimension);

private:
    mask_fn isMasked;
    
    explicit DataMask (mask_fn is_masked) : 
        isMasked{is_masked}
    {
        // blank
    }
};

} // ns qrcode
} // ns qrcode

#endif // __DATA_MASK_H__

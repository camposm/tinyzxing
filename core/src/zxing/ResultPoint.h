#ifndef __RESULT_POINT_H__
#define __RESULT_POINT_H__

/*
 *  ResultPoint.h
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

#include <utility>
#include <array>

namespace zxing {

using ResultPoint = std::pair<float, float>;

namespace resultpoint {

void orderBestPatterns (std::array<ResultPoint, 3>& patterns);
float distance (const ResultPoint& point1, const ResultPoint& point2);
float distance (float x1, float x2, float y1, float y2);

} // ns resultpoint
} // ns zxing

#endif // __RESULT_POINT_H__

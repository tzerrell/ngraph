//*****************************************************************************
// Copyright 2017-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include "ngraph/shape.hpp"

namespace ngraph
{
    namespace runtime
    {
        namespace gcpu
        {
            namespace kernel
            {
                template <typename T>
                void result(const T* arg, T* out, size_t count)
                {
                    memcpy(out, arg, sizeof(T) * count);
                }
            }
        }
    }
}

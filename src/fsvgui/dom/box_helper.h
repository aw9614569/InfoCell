#pragma once
#include <vector>

namespace fsvgui {
namespace box_helper {

struct Element
{
    // Input:
    int min_size    = 0;
    int flex_grow   = 0;
    int flex_shrink = 0;

    // Output;
    int size = 0;
};

void Compute(std::vector<Element>* elements, int target_size);

} // namespace box_helper
} // namespace fsvgui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.line.
